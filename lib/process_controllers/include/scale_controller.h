#ifndef SCALE_CONTROLLER_H
#define SCALE_CONTROLLER_H

#include <distillation_state_manager.h>
#include <logger.h>
#include <scale.h>

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

#if !defined(UNIT_TEST) && !defined(NATIVE)
  Logger *logger = nullptr; /**< Logger for recording events. */
#else
  Logger *logger = nullptr; /**< Logger for recording events (null in test/native). */
#endif

public:
#if defined(UNIT_TEST) || defined(NATIVE)
  /**
   * Constructor for the ScaleController class in test/native environment.
   * @param earlyForeshotsScale Scale for weighing the early foreshots.
   * @param lateForeshotsScale Scale for weighing the late foreshots.
   * @param headsScale Scale for weighing the heads.
   * @param heartsScale Scale for weighing the hearts.
   * @param earlyTailsScale Scale for weighing the early tails.
   * @param lateTailsScale Scale for weighing the late tails.
   * @param logger Pointer to the logger instance (optional).
   */
  ScaleController(Scale &earlyForeshotsScale, Scale &lateForeshotsScale, Scale &headsScale, Scale &heartsScale,
                  Scale &earlyTailsScale, Scale &lateTailsScale, Logger *logger = nullptr)
    : earlyForeshotsScale(earlyForeshotsScale), lateForeshotsScale(lateForeshotsScale), headsScale(headsScale),
      heartsScale(heartsScale), earlyTailsScale(earlyTailsScale), lateTailsScale(lateTailsScale), logger(logger) {}
#else
  /**
   * Constructor for the ScaleController class.
   * @param earlyForeshotsScale Scale for weighing the early foreshots.
   * @param lateForeshotsScale Scale for weighing the late foreshots.
   * @param headsScale Scale for weighing the heads.
   * @param heartsScale Scale for weighing the hearts.
   * @param earlyTailsScale Scale for weighing the early tails.
   * @param lateTailsScale Scale for weighing the late tails.
   * @param logger Pointer to the logger instance (optional).
   */
  ScaleController(Scale &earlyForeshotsScale, Scale &lateForeshotsScale, Scale &headsScale, Scale &heartsScale,
                  Scale &earlyTailsScale, Scale &lateTailsScale, Logger *logger = nullptr)
    : earlyForeshotsScale(earlyForeshotsScale), lateForeshotsScale(lateForeshotsScale), headsScale(headsScale),
      heartsScale(heartsScale), earlyTailsScale(earlyTailsScale), lateTailsScale(lateTailsScale), logger(logger) {

    if (logger) {
      logger->info("ScaleController initialized");

      // Log initial connection status of all scales
      logScaleStatus(EARLY_FORESHOTS, earlyForeshotsScale);
      logScaleStatus(LATE_FORESHOTS, lateForeshotsScale);
      logScaleStatus(HEADS, headsScale);
      logScaleStatus(HEARTS, heartsScale);
      logScaleStatus(EARLY_TAILS, earlyTailsScale);
      logScaleStatus(LATE_TAILS, lateTailsScale);
    }
  }
#endif

  /**
   * Updates all the scales' weights.
   */
  void updateAllWeights() {
#if !defined(UNIT_TEST) && !defined(NATIVE)
    if (logger) {
      logger->debug("Updating all scales");
    }

    // Update each scale and log any failures
    updateAndLogScale(EARLY_FORESHOTS, earlyForeshotsScale);
    updateAndLogScale(LATE_FORESHOTS, lateForeshotsScale);
    updateAndLogScale(HEADS, headsScale);
    updateAndLogScale(HEARTS, heartsScale);
    updateAndLogScale(EARLY_TAILS, earlyTailsScale);
    updateAndLogScale(LATE_TAILS, lateTailsScale);
#else
    // In test/native environment, just update all scales
    earlyForeshotsScale.updateWeight();
    lateForeshotsScale.updateWeight();
    headsScale.updateWeight();
    heartsScale.updateWeight();
    earlyTailsScale.updateWeight();
    lateTailsScale.updateWeight();
#endif
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
#if !defined(UNIT_TEST) && !defined(NATIVE)
      if (logger) {
        logger->warning("Attempted to get weight for invalid state: %d", static_cast<int>(state));
      }
#endif
      return -1.0; // Handle invalid state
    }
  }

  /**
   * Attempts to reconnect all disconnected scales.
   * @return Number of scales successfully reconnected.
   */
  int tryReconnectScales() {
#if !defined(UNIT_TEST) && !defined(NATIVE)
    int reconnectedCount = 0;

    if (logger) {
      logger->info("Attempting to reconnect all disconnected scales");
    }

    // Try to reconnect each disconnected scale
    if (!earlyForeshotsScale.isConnected() && earlyForeshotsScale.tryReconnect())
      reconnectedCount++;
    if (!lateForeshotsScale.isConnected() && lateForeshotsScale.tryReconnect())
      reconnectedCount++;
    if (!headsScale.isConnected() && headsScale.tryReconnect())
      reconnectedCount++;
    if (!heartsScale.isConnected() && heartsScale.tryReconnect())
      reconnectedCount++;
    if (!earlyTailsScale.isConnected() && earlyTailsScale.tryReconnect())
      reconnectedCount++;
    if (!lateTailsScale.isConnected() && lateTailsScale.tryReconnect())
      reconnectedCount++;

    if (logger) {
      logger->info("Reconnected %d scales", reconnectedCount);
    }

    return reconnectedCount;
#else
    return 0;
#endif
  }

  /**
   * Get the number of connected scales.
   * @return Number of connected scales (0-6).
   */
  int getConnectedScaleCount() const {
    int count = 0;
    if (earlyForeshotsScale.isConnected())
      count++;
    if (lateForeshotsScale.isConnected())
      count++;
    if (headsScale.isConnected())
      count++;
    if (heartsScale.isConnected())
      count++;
    if (earlyTailsScale.isConnected())
      count++;
    if (lateTailsScale.isConnected())
      count++;
    return count;
  }

  /**
   * Check if a specific scale is connected.
   * @param state The distillation state corresponding to the scale.
   * @return True if connected, false otherwise.
   */
  bool isScaleConnected(DistillationState state) const {
    switch (state) {
    case EARLY_FORESHOTS:
      return earlyForeshotsScale.isConnected();
    case LATE_FORESHOTS:
      return lateForeshotsScale.isConnected();
    case HEADS:
      return headsScale.isConnected();
    case HEARTS:
      return heartsScale.isConnected();
    case EARLY_TAILS:
      return earlyTailsScale.isConnected();
    case LATE_TAILS:
      return lateTailsScale.isConnected();
    default:
      return false;
    }
  }

private:
#if !defined(UNIT_TEST) && !defined(NATIVE)
  /**
   * Update a scale and log any failures.
   * @param state The distillation state corresponding to the scale.
   * @param scale The scale to update.
   */
  void updateAndLogScale(DistillationState state, Scale &scale) {
    bool success = scale.updateWeight();

    if (!success && logger) {
      logger->warning("Failed to update scale for state: %s", stateToString(state));
    }
  }

  /**
   * Log the connection status of a scale.
   * @param state The distillation state corresponding to the scale.
   * @param scale The scale to check.
   */
  void logScaleStatus(DistillationState state, const Scale &scale) {
    if (!logger)
      return;

    if (scale.isConnected()) {
      logger->info("Scale for state %s is connected", stateToString(state));
    } else {
      logger->warning("Scale for state %s is not connected", stateToString(state));
    }
  }

  /**
   * Convert distillation state to string.
   * @param state The distillation state.
   * @return String representation of the state.
   */
  const char *stateToString(DistillationState state) const {
    switch (state) {
    case EARLY_FORESHOTS:
      return "EARLY_FORESHOTS";
    case LATE_FORESHOTS:
      return "LATE_FORESHOTS";
    case HEADS:
      return "HEADS";
    case HEARTS:
      return "HEARTS";
    case EARLY_TAILS:
      return "EARLY_TAILS";
    case LATE_TAILS:
      return "LATE_TAILS";
    case OFF:
      return "OFF";
    case HEAT_UP:
      return "HEAT_UP";
    case STABILIZING:
      return "STABILIZING";
    case FINALIZING:
      return "FINALIZING";
    default:
      return "UNKNOWN";
    }
  }
#endif
};

#endif // SCALE_CONTROLLER_H
