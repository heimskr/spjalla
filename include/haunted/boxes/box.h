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
			int size;
			box(int size_): size(size_) {}

		public:
			haunted::side side;

			virtual int size_one() const;
			virtual int size_two() const;
			virtual void set_size(int);
	};
}

#endif
