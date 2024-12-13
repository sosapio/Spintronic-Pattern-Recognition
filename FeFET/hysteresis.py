from collections import deque
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.optimize import minimize

def generate_schmitt_hysteresis_loop(low_threshold, high_threshold, time_steps=100):
    def schmitt_trigger(input_signal, low_threshold, high_threshold):
        output_signal = np.zeros_like(input_signal)
        state = 0  # Initial state

        for i in range(1, len(input_signal)):
            if input_signal[i] > high_threshold:
                state = 1
            elif input_signal[i] < low_threshold:
                state = 0
            output_signal[i] = state

        return output_signal

    # Electric field (time-varying)
    E = np.sin(np.linspace(0, 10, time_steps))  # Example varying electric field

    # Generate hysteresis loop using Schmitt trigger
    P = schmitt_trigger(E, low_threshold, high_threshold)

    return E, P

def generate_hysteresis_loop(Ps, Ec, lambda_, rate_of_change, remanent_polarization, time_steps=100, window_size=10):
    # Electric field (time-varying)
    E = np.sin(np.linspace(0, 10, time_steps))  # Example varying electric field

    # Initialize polarization and memory arrays
    P = np.zeros(time_steps)
    P_window = deque(maxlen=window_size)
    memory_term = 0  # Initialize the memory term

    for n in range(1, time_steps):
        if len(P_window) > 0:
            memory_term = sum(P_window) / len(P_window)
        else:
            memory_term = 0

        # Scale the tanh curve by remanent_polarization
        P[n] = remanent_polarization * (Ps * np.tanh(E[n] / (Ec * rate_of_change) + lambda_ * memory_term))

        P_window.append(P[n - 1])

    return E, P

def calculate_rmse(experimental_E, experimental_P, model_E, model_P):
    interpolated_model_P = np.interp(experimental_E, model_E, model_P)
    
    # Calculate the error between experimental and interpolated model polarization
    error = experimental_P - interpolated_model_P
    
    # Calculate RMSE
    rmse = np.sqrt(np.mean(error ** 2))
    
    return rmse

def objective_function(params):
    Ps, Ec, lambda_, rate_of_change, remanent_polarization = params
    model_E, model_P = generate_hysteresis_loop(Ps, Ec, lambda_, rate_of_change, remanent_polarization)
    rmse = calculate_rmse(experimental_E, experimental_P, model_E, model_P)
    return rmse

# Load experimental data
experimental_data = pd.read_csv('bkt_hyst.csv')
experimental_E = experimental_data['E'].values
experimental_P = experimental_data['P'].values

# Initial guess for parameters Ps, Ec, lambda_, rate_of_change, remanent_polarization
initial_guess = [30, 5, 3, 3, 1]

# Generate hysteresis loop with initial guess
model_E_initial, model_P_initial = generate_hysteresis_loop(*initial_guess)

# Optimize parameters to minimize RMSE
result = minimize(objective_function, initial_guess, method='Nelder-Mead')
optimized_params = result.x

# Generate hysteresis loop with optimized parameters
Ps_opt, Ec_opt, lambda_opt, rate_of_change_opt, remanent_polarization_opt = optimized_params
model_E_opt, model_P_opt = generate_hysteresis_loop(Ps_opt, Ec_opt, lambda_opt, rate_of_change_opt, remanent_polarization_opt)

rmse_initial = calculate_rmse(experimental_E, experimental_P, model_E_initial, model_P_initial)
rmse_optimized = calculate_rmse(experimental_E, experimental_P, model_E_opt, model_P_opt)

# Plot experimental data, initial guess, and optimized model data
plt.figure()
plt.plot(experimental_E, experimental_P, 'o', label='Experimental Data')
plt.plot(model_E_initial, model_P_initial, '--', label='Initial Guess')
plt.plot(model_E_opt, model_P_opt, '-', label='Optimized Model')

# Add Schmitt trigger hysteresis loop (optional)
# low_threshold = -0.5
# high_threshold = 0.5
# E_schmitt, P_schmitt = generate_schmitt_hysteresis_loop(low_threshold, high_threshold)
# plt.plot(E_schmitt, P_schmitt, label='Schmitt Trigger', linestyle='--')

plt.axhline(0, color='black', linewidth=1.5)
plt.axvline(0, color='black', linewidth=1.5)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)

plt.xlabel('Electric Field (E)')
plt.ylabel('Polarization (P)')
plt.title('Hysteresis Loops with Corrected Remanent Polarization Scaling')
plt.legend()
plt.grid()
plt.show()

# Print optimized parameters
print(f'Optimized Parameters:\nPs: {Ps_opt}\nEc: {Ec_opt}\nlambda: {lambda_opt}\nrate_of_change: {rate_of_change_opt}\nremanent_polarization: {remanent_polarization_opt}')
print(f'Initial Guess RMSE: {rmse_initial:.4f}')
print(f'Optimized RMSE: {rmse_optimized:.4f}')
