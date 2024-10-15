#ifndef SCALE_CONTROLLER_H
#define SCALE_CONTROLLER_H

#include "scale.h"
#include "distillation_state_manager.h"

/**
 * Class for managing scales.
 */
class ScaleController {
private:
  Scale earlyForeshotsScale; /**< Scale for weighing the early foreshots. */
  Scale lateForeshotsScale;  /**< Scale for weighing the late foreshots. */
  Scale headsScale;          /**< Scale for weighing the heads. */
  Scale heartsScale;         /**< Scale for weighing the hearts. */
  Scale earlyTailsScale;     /**< Scale for weighing the early tails. */
  Scale lateTailsScale;      /**< Scale for weighing the late tails. */

public:
  /**
   * Constructor for the ScaleController class.
   * @param earlyForeshotsScale Scale for weighing the early foreshots.
   * @param lateForeshotsScale Scale for weighing the late foreshots.
   * @param headsScale Scale for weighing the heads.
   * @param heartsScale Scale for weighing the hearts.
   * @param earlyTailsScale Scale for weighing the early tails.
   * @param lateTailsScale Scale for weighing the late tails.
   */
  ScaleController(const Scale &earlyForeshotsScale, const Scale &lateForeshotsScale, const Scale &headsScale,
                  const Scale &heartsScale, const Scale &earlyTailsScale, const Scale &lateTailsScale)
    : earlyForeshotsScale(earlyForeshotsScale), lateForeshotsScale(lateForeshotsScale), headsScale(headsScale),
      heartsScale(heartsScale), earlyTailsScale(earlyTailsScale), lateTailsScale(lateTailsScale) {}

  /**
   * Returns the weight of the distillate for the provided state.
   * @param state The distillate state for which to get the weight.
   * @return The weight of the distillate.
   */
  double getWeight(DistillationState state) const {
    switch (state) {
    case EARLY_FORESHOTS:
      return earlyForeshotsScale.getWeight();
    case LATE_FORESHOTS:
      return lateForeshotsScale.getWeight();
    case HEADS:
      return headsScale.getWeight();
    case HEARTS:
      return heartsScale.getWeight();
    case EARLY_TAILS:
      return earlyTailsScale.getWeight();
    case LATE_TAILS:
      return lateTailsScale.getWeight();
    default:
      // Handle invalid state
      return -1.0;
    }
  }
};

#endif // SCALE_CONTROLLER_H
