# Update March 2018

This code was written in 2005 for the CD-ROM that accompanied the book AI Game Programming Wisdom 3: http://www.aiwisdom.com/resource_aiwisdom3.html. 

It's the companion to the article Intelligent Steering Using Adaptive PID Controllers. 

---

An earlier version of this demo can be found here: https://github.com/euan-forrester/ai-wisdom-2-sample

---

## Requirements

This sample requires Windows XP to run, and MS VC.Net 2003 to compile

## Usage notes

- The demo starts off with a non-adaptive controller to give you a feel for what's going on. The missile is attempting to steer towards the target, and you can affect the handling of the missile by tuning the "Missile Rotational Drag," "Missile Max Angular Acceleration," and "Missile PID output scale" sliders. Notice how when you change the settings, the missile no longer is able to track the target as effectively.

- Click on the "?" buttons next to the P, I, and D sliders for a brief description of their functions.

- You can move the P, I, and D sliders to attempt to re-tune the missile's steering to match the new handling if you wish.

- You can also choose to steer the missile using the keyboard, use 8 key on the numpad to accelerate, and use 4 and 6 to steer left and right. Hopefully, you will discover that the missile is actually quite difficult to control.

- Now put the missile into adaptive control mode and watch the P, I, and D move themselves in an effort to make the missile track the target well again. If you've made a small change to the missile's handling, the sliders should re-tune themselves fairly quickly. If you've made a larger change, it may take longer, or may never even reach equilibrium.

- Click on the Reset button to reset all of the sliders to their default values.

- Click on the Pause button to pause the demo if you want to change the value of several sliders at once.

- Many tuning values, such as the round-robin timeslice, and the clamps on the P, I, and D coefficents, can be found at the top of MainDlg.cpp. The model can be found at the top of CMissile.cpp. These tuning values are present only in the code to avoid cluttering the GUI.

- Because this demo is so simple, the D term has by far the largest effect on the missile's handling; enough damping is sufficient to correct the missile's behavior no matter how its handling is set. Also, there are no external forces to induce steady-state error, and so the I term is not very useful. Nevertheless, the basics of model reference adaptive control are still shown, and the code can be used as the starting point for a more robust application.
