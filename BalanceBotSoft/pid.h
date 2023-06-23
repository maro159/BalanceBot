/*	Floating point PID control loop for Microcontrollers
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx


    github.com/geekfactory/PID
    Modified by maro159
 */

/* NOTE:
 * uint32_t is used to stored time to improve performance.
 * It wraps up every 1 hour 11 minutes and 35 seconds.
 * It doesn't matter since because U2 subtraction is used to calculate time change
 * */

#ifndef _PID_H_
#define _PID_H_

#include "pico/stdlib.h"

enum pid_control_directions 
{
	E_PID_DIRECT,
	E_PID_REVERSE,
};

enum pid_mode
{
	E_MODE_MANUAL,
	E_MODE_AUTO,
};

/**
 * Structure that holds PID all the PID controller data, multiple instances are
 * posible using different structures for each controller
 */
struct pid_controller 
{
	/*  Input, output and setpoint */
	float * input; /*  Current Process Value */
	float * output; /*  Corrective Output from PID Controller */
	float * setpoint; /*  Controller Setpoint */
	/*  Tuning parameters */
	float Kp; /*  Stores the gain for the Proportional term */
	float Ki; /*  Stores the gain for the Integral term */
	float Kd; /*  Stores the gain for the Derivative term */
	/*	Tuning parameters to display	*/
	float kp_disp;
	float ki_disp;
	float kd_disp;
	/*  Output minimum and maximum values */
	float omin; /*  Maximum value allowed at the output */
	float omax; /*  Minimum value allowed at the output */
	/*  Variables for PID algorithm */
	float iterm; /*  Accumulator for integral term */
	float lastin; /*  Last input value for differential term */
	/*  Time related */
	uint32_t lasttime_us; /*  Stores the time when the control loop ran last time */
	uint32_t sampletime_us; /*  Defines the PID sample time in us */
	/*  Operation mode */
	enum pid_mode mode; /*  Defines if the PID controller is enabled or disabled */
	enum pid_control_directions direction;
};

typedef struct pid_controller * pid_ctrl;

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
	/**
	 * @brief Creates a new PID controller
	 *
	 * Creates a new pid controller and initializes it�s input, output and internal
	 * variables. Also we set the tuning parameters
	 *
	 * @param pid A pointer to a pid_controller structure
	 * @param in Pointer to float value for the process input
	 * @param out Poiter to put the controller output value
	 * @param set Pointer float with the process setpoint value
	 * @param kp Proportional gain
	 * @param ki Integral gain
	 * @param kd Diferential gain
	 *
	 * @return returns a pid_ctrl controller handle
	 */
	pid_ctrl pid_create(pid_ctrl pid, float* in, float* out, float* set, float kp, float ki, float kd);

	/**
	 * @brief Check if PID loop needs to run
	 *
	 * Determines if the PID control algorithm should compute a new output value,
	 * if this returs true, the user should read process feedback (sensors) and
	 * place the reading in the input variable, then call the pid_compute() function.
	 *
	 * @return return Return true if PID control algorithm is required to run
	 */
	bool pid_need_compute(pid_ctrl pid);

	/**
	 * @brief Computes the output of the PID control
	 *
	 * This function computes the PID output based on the parameters, setpoint and
	 * current system input.
	 *
	 * @param pid The PID controller instance which will be used for computation
	 */
	void pid_compute(pid_ctrl pid);

	/**
	 * @brief Sets new PID tuning parameters
	 *
	 * Sets the gain for the Proportional (Kp), Integral (Ki) and Derivative (Kd)
	 * terms.
	 *
	 * @param pid The PID controller instance to modify
	 * @param kp Proportional gain
	 * @param ki Integral gain
	 * @param kd Derivative gain
	 */
	void pid_tune(pid_ctrl pid, float kp, float ki, float kd);

	/**
	 * @brief Sets the pid algorithm period
	 *
	 * Changes the between PID control loop computations.
	 *
	 * @param pid The PID controller instance to modify
	 * @param time The time in milliseconds between computations
	 */
	void pid_sample(pid_ctrl pid, uint32_t time);

	/**
	 * @brief Sets the limits for the PID controller output
	 *
	 * @param pid The PID controller instance to modify
	 * @param min The minimum output value for the PID controller
	 * @param max The maximum output value for the PID controller
	 */
	void pid_limits(pid_ctrl pid, float min, float max);

	/**
	 * @brief Configures the PID controller mode
	 *
	 * In manual mode user can modify the value of the output
	 * variable and the controller will not overwrite it.
	 * In auto mode control loop is enabled and user cannot overwrite the output
	 *
	 * @param pid The PID controller instance to disable
	 * @param mode Mod to set: manual or auto 
	 */
	void pid_set_mode(pid_ctrl pid, enum pid_mode mode);

	/**
	 * @brief Configures the PID controller direction
	 *
	 * Sets the direction of the PID controller. The direction is "DIRECT" when a
	 * increase of the output will cause a increase on the measured value and
	 * "REVERSE" when a increase on the controller output will cause a decrease on
	 * the measured value.
	 *
	 * @param pid The PID controller instance to modify
	 * @param direction The new direction of the PID controller
	 */
	void pid_direction(pid_ctrl pid, enum pid_control_directions dir);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PID_H_ */