#pragma once
#include "../math/vec3.hpp"
#include <algorithm>
#include <stdexcept>

class RocketBody {
private:
  double length_;          // Length of rocket (m)
  double diameter_;        // Diameter of rocket (m)
  double referenceArea_;   // Reference area (m²)
  double dragCoefficient_; // Base drag coefficient
  double liftCoefficient_; // Base lift coefficient
  Vec3 centerOfMass_;      // Center of mass position
  double mass_;            // Current mass (kg)
  double wetMass_;         // Mass with full fuel (kg)
  double dryMass_;         // Mass without fuel (kg)
  double fuelMass_;        // Current fuel mass (kg)

public:
  RocketBody(double len, double dia, double wetM, double dryM)
      : length_(len), diameter_(dia), referenceArea_(3.14159 * dia * dia / 4.0),
        dragCoefficient_(0.0), liftCoefficient_(0.0), centerOfMass_(0, 0, 0),
        mass_(wetM), wetMass_(wetM), dryMass_(dryM), fuelMass_(wetM - dryM) {
    if (dryMass_ >= wetMass_) {
      throw std::invalid_argument("Dry mass must be less than wet mass");
    }
    if (dryMass_ <= 0 || wetMass_ <= 0) {
      throw std::invalid_argument("Masses must be positive");
    }
  }

  // Getters
  double getLength() const { return length_; }
  double getDiameter() const { return diameter_; }
  double getReferenceArea() const { return referenceArea_; }
  double getDragCoefficient() const { return dragCoefficient_; }
  double getLiftCoefficient() const { return liftCoefficient_; }
  double getMass() const { return mass_; }
  double getWetMass() const { return wetMass_; }
  double getDryMass() const { return dryMass_; }
  double getCurrentFuelMass() const { return fuelMass_; }
  Vec3 getCenterOfMass() const { return centerOfMass_; }

  // Calculate remaining fuel percentage
  double getFuelRemainingPercentage() const {
    return (fuelMass_ / (wetMass_ - dryMass_)) * 100.0;
  }

  // Update mass based on remaining fuel ratio
  void updateMass(double fuelRatio) {
    // Ensure ratio is between 0 and 1
    fuelRatio = std::clamp(fuelRatio, 0.0, 1.0);

    // Calculate new fuel mass
    fuelMass_ = (wetMass_ - dryMass_) * fuelRatio;

    // Update total mass
    mass_ = dryMass_ + fuelMass_;
  }

  void updateAeroCoefficients(double machNumber, double angleOfAttack) {
    // Simple subsonic-transonic-supersonic drag model
    if (machNumber < 0.8)
      dragCoefficient_ = 0.2;
    else if (machNumber < 1.2)
      dragCoefficient_ = 0.2 + 0.6 * (machNumber - 0.8);
    else
      dragCoefficient_ = 0.4;

    // Simple lift model based on angle of attack
    liftCoefficient_ = 0.1 * std::sin(2 * angleOfAttack);
  }

  // Calculate center of mass based on fuel level
  void updateCenterOfMass() {
    // Simplified model: CoM moves along the length as fuel is consumed
    // Assume fuel tank is in the upper half of the rocket
    double fuelRatio = fuelMass_ / (wetMass_ - dryMass_);
    double comZ =
        length_ * (0.4 + 0.2 * fuelRatio); // CoM between 40% and 60% of length
    centerOfMass_ = Vec3(0, 0, comZ);
  }
};
