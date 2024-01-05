/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <acpi/hpet.h>
#include <proc/eb.h>
#include <proc/scheduler.h>
#include <proc/tasking.h>

#include <luxe.h>

static vector_struct(task_event_t) g_eb_publisher_list = { 0 };
static vector_struct(task_event_t) g_eb_subscriber_list = { 0 };

lock_t eb_lock = lock_create();

bool eb_publish(uint64_t id, event_type_t type, uint64_t para)
{
	if (type != EVENT_KEYPRESS) {
		return false;
	}

	task_event_t e = {
		.pub_id = id, .sub_id = TASKID_MAX, .type = type, .para = para
	};
	e.timestamp = hpet_get_ns();

	lock_acquire(&eb_lock);
	vector_pushback(&g_eb_publisher_list, e);
	lock_release(&eb_lock);

#ifdef CONFIG_DEBUG
	klog("task %d published\tpara 0x%.8llx, type 0x%.8llx, millis %d, ticks %d",
		 id, para, type, hpet_get_ns() / 1000000, _sched_get_ticks());
#endif

	return true;
}

bool eb_subscribe(uint64_t id, event_type_t type, uint64_t *para)
{
	if (type != EVENT_KEYPRESS) {
		return false;
	}

	task_event_t e = {
		.pub_id = TASKID_MAX, .sub_id = id, .type = type, .para = 0
	};
	e.timestamp = hpet_get_ns();

	lock_acquire(&eb_lock);
	vector_pushback(&g_eb_subscriber_list, e);
	lock_release(&eb_lock);

	e = sched_wait_for_event(e);
	*para = e.para;

#ifdef CONFIG_DEBUG
	klog(
		"task %d subscribed\tpara 0x%.8llx, type 0x%.8llx, millis %d, ticks %d",
		id, para, type, hpet_get_ns() / 1000000, _sched_get_ticks());
#endif

	return true;
}

void eb_dispatch(void)
{
	lock_acquire(&eb_lock);

	for (;;) {
		if (vector_length(&g_eb_publisher_list) > 0) {
			task_event_t e = vector_at(&g_eb_publisher_list, 0);
			if (e.type != EVENT_KEYPRESS) {
				vector_erase(&g_eb_publisher_list, 0);
				continue;
			}
			if (sched_resume_event(e)) {
				vector_erase(&g_eb_publisher_list, 0);
			}
			break;
		} else {
			break;
		}
	}

	lock_release(&eb_lock);
}