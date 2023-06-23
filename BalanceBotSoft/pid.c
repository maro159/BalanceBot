#include <stdio.h>
#include "pico/stdlib.h"
#include "pid.h"
#include "hardware/timer.h"

/*	Floating point PID control loop for Microcontrollers
	Copyright (C) 2015 Jesus Ruben Santa Anna Zamudio.

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
 
    modified by maro159
 */

pid_ctrl pid_create(pid_ctrl pid, float* in, float* out, float* set, float kp, float ki, float kd)
{
	pid->input = in;
	pid->output = out;
	pid->setpoint = set;
	pid->mode = E_MODE_MANUAL;
    pid->kp_disp = kp;
    pid->ki_disp = ki;
    pid->kd_disp = kd;

	pid_limits(pid, 0, 255);

	/* Set default sample time to 100 ms */
	pid->sampletime_us = 100 * 1000;

	pid_direction(pid, E_PID_DIRECT);
	pid_tune(pid, kp, ki, kd);

	pid->lasttime_us = time_us_32() - pid->sampletime_us;

	return pid;
}

bool pid_need_compute(pid_ctrl pid)
{
	/* Check if the PID period has elapsed */
	return(time_us_32() - pid->lasttime_us >= pid->sampletime_us) ? true : false;
}

void pid_compute(pid_ctrl pid)
{
	/* Check if control is enabled */
	if (pid->mode == E_MODE_MANUAL)
        {
            return;
        }
	
	float in = *(pid->input);
	/* Compute error */
	float error = (*(pid->setpoint)) - in;
	/* Compute integral */
	pid->iterm += (pid->Ki * error);
	if (pid->iterm > pid->omax)
		pid->iterm = pid->omax;
	else if (pid->iterm < pid->omin)
		pid->iterm = pid->omin;
	/* Compute differential on input */
	float dinput = in - pid->lastin;
	/* Compute PID output */
	float out = pid->Kp * error + pid->iterm - pid->Kd * dinput;
	/* Apply limit to output value */
	if (out > pid->omax)
		out = pid->omax;
	else if (out < pid->omin)
		out = pid->omin;
	/* Output to pointed variable */
	(*pid->output) = out;
	/* Keep track of some variables for next execution */
	pid->lastin = in;
	pid->lasttime_us = time_us_32();
}

void pid_tune(pid_ctrl pid, float kp, float ki, float kd)
{
	/* Check for validity */
	if (kp < 0 || ki < 0 || kd < 0)
		return;
	
	// Compute sample time in seconds
	float sampletime_sec = ((float) pid->sampletime_us) / ((float) (1000 * 1000));

    // Save coeffs as entered 
    pid->kp_disp = kp;
    pid->ki_disp = ki;
    pid-> kd_disp = kd;
    
    // Save coeffs for calculations
    pid->Kp = kp;
	pid->Ki = ki * sampletime_sec;
	pid->Kd = kd / sampletime_sec;

	if (pid->direction == E_PID_REVERSE)
    {
		pid->Kp = 0 - pid->Kp;
		pid->Ki = 0 - pid->Ki;
		pid->Kd = 0 - pid->Kd;
	}
}

void pid_sample(pid_ctrl pid, uint32_t new_sampletime_us)
{
	if (new_sampletime_us > 0)
    {
		float ratio = (float)(new_sampletime_us) / (float)(pid->sampletime_us);
		pid->Ki *= ratio;
		pid->Kd /= ratio;
		pid->sampletime_us = new_sampletime_us;
	}
}

void pid_limits(pid_ctrl pid, float min, float max)
{
	if (min >= max) return;
	pid->omin = min;
	pid->omax = max;
	//Adjust output to new limits
	if (pid->mode == E_MODE_AUTO) 
    {
		if (*(pid->output) > pid->omax)
			*(pid->output) = pid->omax;
		else if (*(pid->output) < pid->omin)
			*(pid->output) = pid->omin;

		if (pid->iterm > pid->omax)
			pid->iterm = pid->omax;
		else if (pid->iterm < pid->omin)
			pid->iterm = pid->omin;
	}
}

void pid_set_mode(pid_ctrl pid, enum pid_mode new_mode)
{
    if (pid->mode == E_MODE_MANUAL && new_mode == E_MODE_AUTO)
    {
        pid->iterm = *(pid->output);
		pid->lastin = *(pid->input);
		if (pid->iterm > pid->omax)
			pid->iterm = pid->omax;
		else if (pid->iterm < pid->omin)
			pid->iterm = pid->omin;
    }
    pid->mode = new_mode;
}

void pid_direction(pid_ctrl pid, enum pid_control_directions dir)
{
	if ((pid-> mode == E_MODE_AUTO) && (pid->direction != dir)) // TODO: what if direction changed in manual mode 
    {
		pid->Kp = (0 - pid->Kp);
		pid->Ki = (0 - pid->Ki);
		pid->Kd = (0 - pid->Kd);
	}
	pid->direction = dir;
}

