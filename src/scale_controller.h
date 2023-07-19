#ifndef SCALE_CONTROLLER_H
#define SCALE_CONTROLLER_H

#include "Scale.h"
#include "distillation_state_manager.h"

/**
 * Class for managing scales.
 */
class ScaleController {
private:
  Scale foreshotsScale; /**< Scale for weighing the foreshots. */
  Scale headsScale;     /**< Scale for weighing the heads. */
  Scale heartsScale;    /**< Scale for weighing the hearts. */
  Scale tailsScale;     /**< Scale for weighing the tails. */

public:
  /**
   * Constructor for the ScaleController class.
   * @param foreshotsScale Scale for weighing the foreshots.
   * @param headsScale Scale for weighing the heads.
   * @param heartsScale Scale for weighing the hearts.
   * @param tailsScale Scale for weighing the tails.
   */
  ScaleController(const Scale &foreshotsScale, const Scale &headsScale, const Scale &heartsScale,
                  const Scale &tailsScale)
    : foreshotsScale(foreshotsScale), headsScale(headsScale), heartsScale(heartsScale), tailsScale(tailsScale) {}

  /**
   * Returns the weight of the distillate for the provided state.
   * @param state The distillate state for which to get the weight.
   * @return The weight of the distillate.
   */
  double getWeight(DistillationState state) const {
    switch (state) {
    case FORESHOTS:
      return foreshotsScale.getWeight();
    case HEADS:
      return headsScale.getWeight();
    case HEARTS:
      return heartsScale.getWeight();
    case TAILS:
      return tailsScale.getWeight();
    default:
      // Handle invalid state
      return -1.0;
    }
  }
};

#endif // SCALE_CONTROLLER_H
