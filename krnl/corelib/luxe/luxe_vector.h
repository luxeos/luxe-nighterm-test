/**
 * LuxeOS (c) 2023 by Jozef Nagy
 *
 * LuxeOS is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LUXE_VECTOR_H_
#define __LUXE_VECTOR_H_

#define vector_struct(type) \
	struct {                \
		size_t len;         \
		size_t cap;         \
		type *data;         \
	}

#define vector_pushback(vec, elem)                           \
	{                                                        \
		(vec)->len++;                                        \
		if ((vec)->cap < (vec)->len * sizeof(elem)) {        \
			(vec)->cap = (vec)->len * sizeof(elem) * 4;      \
			(vec)->data = krealloc((vec)->data, (vec)->cap); \
		}                                                    \
		(vec)->data[(vec)->len - 1] = elem;                  \
	}

#define vector_length(vec) (vec)->len
#define vector_at(vec, index) (vec)->data[index]

#define vector_erase(vec, index)                                 \
	{                                                            \
		memcpy(&((vec)->data[index]), &((vec)->data[index + 1]), \
			   sizeof((vec)->data[0]) * (vec)->len - index - 1); \
		(vec)->len--;                                            \
	}

#define vector_erase_all(vec)    \
	{                            \
		(vec)->len = 0;          \
		(vec)->cap = 0;          \
		if ((vec)->data != NULL) \
			kfree((vec)->data);  \
		(vec)->data = NULL;      \
	}

#define vector_erase_val(vec, val)                      \
	{                                                   \
		for (size_t __i = 0; __i < (vec)->len; __i++) { \
			if (vector_at(vec, __i) == (val)) {         \
				vector_erase(vec, __i);                 \
				break;                                  \
			}                                           \
		}                                               \
	}

#endif /* __LUXE_VECTOR_H_ */