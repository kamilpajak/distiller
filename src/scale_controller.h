#ifndef SCALE_CONTROLLER_H
#define SCALE_CONTROLLER_H

#include "distillation_state_manager.h"
#include "scale.h"

/**
 * Class for managing scales.
 */
class ScaleController {
private:
  Scale &earlyForeshotsScale; /**< Scale for weighing the early foreshots. */
  Scale &lateForeshotsScale;  /**< Scale for weighing the late foreshots. */
  Scale &headsScale;          /**< Scale for weighing the heads. */
  Scale &heartsScale;         /**< Scale for weighing the hearts. */
  Scale &earlyTailsScale;     /**< Scale for weighing the early tails. */
  Scale &lateTailsScale;      /**< Scale for weighing the late tails. */

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
  ScaleController(Scale &earlyForeshotsScale, Scale &lateForeshotsScale, Scale &headsScale, Scale &heartsScale,
                  Scale &earlyTailsScale, Scale &lateTailsScale)
    : earlyForeshotsScale(earlyForeshotsScale), lateForeshotsScale(lateForeshotsScale), headsScale(headsScale),
      heartsScale(heartsScale), earlyTailsScale(earlyTailsScale), lateTailsScale(lateTailsScale) {}

  /**
   * Updates all the scales' weights.
   */
  void updateAllWeights() {
    earlyForeshotsScale.updateWeight();
    lateForeshotsScale.updateWeight();
    headsScale.updateWeight();
    heartsScale.updateWeight();
    earlyTailsScale.updateWeight();
    lateTailsScale.updateWeight();
  }

  /**
   * Returns the weight of the distillate for the provided state.
   * @param state The distillate state for which to get the weight.
   * @return The weight of the distillate.
   */
  [[nodiscard]] double getWeight(DistillationState state) const {
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
      return -1.0; // Handle invalid state
    }
  }
};

#endif // SCALE_CONTROLLER_H
