#ifndef HAUNTED_BOXES_BOX_PROPO_H_
#define HAUNTED_BOXES_BOX_PROPO_H_

#include "haunted/boxes/box.h"

namespace haunted::boxes {
	/**
	 * Represents a box whose two children's width are determined with a ratio. 
	 * For example, if the ratio is 1.5, the first child's size will be 1.5x
	 * the second child's size. For a box size of 5, the first child's size will
	 * be 3 and the second child's will be 2.
	 * 
	 *     size_one = ratio * size_box/(1 + ratio)
	 *              = size_box - size_two
	 *              = size_box - size_box/(1 + ratio)
	 *              = size_box * (1 - 1/(1 + ratio))
	 * 
	 *     size_two = size_box/(1 + ratio)
	 */
	class box_propo: public box {
		public:
			double ratio;
			box_propo(const position &, double);

			virtual int size_one() const override;
			virtual int size_two() const override;
	};
}

#endif
