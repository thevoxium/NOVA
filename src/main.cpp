#include "physics/simulationengine.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

int main() {
  try {
    // Initialize rocket configuration
    RocketBody rocket(20.0,    // Length (m)
                      2.0,     // Diameter (m)
                      5000.0,  // Wet mass (kg)
                      2000.0); // Dry mass (kg)

    // Initialize propulsion system
    PropulsionSystem propulsion(3000.0);              // 3000 kg of fuel
    propulsion.addEngine(100000.0, 300.0, 0.5, 20.0); // Add main engine

    // Set initial state (100m above Earth's surface)
    State initialState(Vec3(Constants::EARTH_RADIUS + 100.0, 0, 0), // Position
                       Vec3(0, 0, 0),    // Initial velocity
                       Vec3(),           // Initial acceleration
                       rocket.getMass(), // Initial mass
                       0.0);             // Initial time

    // Initialize simulation
    SimulationEngine sim(initialState, rocket, std::move(propulsion));

    // Start engines at full throttle
    sim.startEngines();
    sim.setThrottle(1.0);

    // Create and setup CSV file
    std::ofstream dataFile("flight_data.csv");
    dataFile << std::fixed << std::setprecision(6);
    dataFile
        << "Time,Altitude,Velocity_X,Velocity_Y,Velocity_Z,Velocity_Magnitude,"
        << "Acceleration_X,Acceleration_Y,Acceleration_Z,Acceleration_"
           "Magnitude,"
        << "Mass,Fuel_Ratio,Air_Density,Air_Pressure,Temperature,"
        << "Dynamic_Pressure,Mach_Number,Drag_Coefficient,Lift_Coefficient\n";

    // Run simulation for 10 seconds
    double endTime = 100.0;
    while (sim.getTime() <= endTime) {
      const State &state = sim.getState();

      // Calculate current conditions
      double altitude = state.position.magnitude() - Constants::EARTH_RADIUS;
      double velocity_mag = state.velocity.magnitude();
      double accel_mag = state.acceleration.magnitude();
      double air_density = Atmosphere::getDensity(altitude);
      double air_pressure = Atmosphere::getPressure(altitude);
      double temperature = Atmosphere::getTemperature(altitude);
      double dynamic_pressure = Aerodynamics::calculateDynamicPressure(state);
      double mach_number =
          velocity_mag /
          std::sqrt(AeroConstants::GAMMA * AeroConstants::AIR_GAS_CONSTANT *
                    temperature);

      // Log data every second
      if (std::fmod(sim.getTime(), 1.0) < 0.01) {
        dataFile << sim.getTime() << "," << altitude << ","
                 << state.velocity.x() << "," << state.velocity.y() << ","
                 << state.velocity.z() << "," << velocity_mag << ","
                 << state.acceleration.x() << "," << state.acceleration.y()
                 << "," << state.acceleration.z() << "," << accel_mag << ","
                 << state.mass << "," << sim.getRemainingFuelRatio() << ","
                 << air_density << "," << air_pressure << "," << temperature
                 << "," << dynamic_pressure << "," << mach_number << ","
                 << rocket.getDragCoefficient() << ","
                 << rocket.getLiftCoefficient() << "\n";

        // Print progress to console
        std::cout << "Time: " << std::setprecision(1) << std::fixed
                  << sim.getTime() << "s, Altitude: " << std::setprecision(1)
                  << altitude << "m, Velocity: " << velocity_mag << "m/s\n";
      }

      // Check if the altitude is below zero
      if (altitude < 0) {
        std::cout << "The rocket has crashed.\n";
        break; // Exit the simulation loop
      }

      sim.step();
    }

    dataFile.close();
    std::cout << "\nSimulation completed. Data saved to flight_data.csv\n";
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
