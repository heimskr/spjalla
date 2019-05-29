#ifndef HAUNTED_BOXES_BOX_H_
#define HAUNTED_BOXES_BOX_H_

#include "haunted/defs.h"
#include "haunted/control.h"

namespace haunted::boxes {
	/**
	 * Base class representing boxes, which are controls that contain
	 * two subcontrols provide calculations for the sizes of the boxes.
	 */
	class box: virtual public control {
		protected:
			box(const position &pos_): control(pos_) {}

		public:
			haunted::side side;

			virtual int max_children() const override;
			virtual int size_one() const;
			virtual int size_two() const;
			virtual int get_size() const;
	};
}

#endif
