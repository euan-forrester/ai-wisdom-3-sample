This code was written in 2005 for the CD-ROM that accompanied the book AI Game Programming Wisdom 3: http://www.aiwisdom.com/resource_aiwisdom3.html. 

It's the companion to the article Intelligent Steering Using Adaptive PID Controllers. 

I also wrote simpler version of this demo showing a non-adaptive PID controller for the preceding article: https://github.com/euan-forrester/ai-wisdom-2-sample

## Some GIFs

#### Beginning the demo

![The demo as when started without changing any settings yet](/images/settings-normal.gif)

The demo opens with the same setup as the previous demo: a missile steered by a non-adaptive PID controller tuned by hand to follow a target.

You can find a more in-depth explanation of how a PID controller works and what's going on here: https://github.com/euan-forrester/ai-wisdom-2-sample

#### Changing how the missile handles

![The missile isn't very good at following the target when we change how it handles](/images/settings-increased-drag.gif)

The physics model used in this demo is very simple so there aren't a lot of ways to change the physical properties of the missile. Along the right side of the window we see the missile's acceleration, its rotational drag, its max angular acceleration, and a scaling factor to apply to the output of the PID controller.

Let's change its rotational drag because that has a big visual impact on how it handles. Now we see that the missile turns very slowly and has a tough time following the target.

#### Allowing our PID controller to adapt

![Letting the missile adapt to the change in its handling](/images/settings-adaptation.gif)

Now let's turn on the adaptive PID controller and see if it will change our PID settings in response to our change in the missile's handling. Watch the 3 sliders on the bottom right, under Missile Steering. It takes a while, but it eventually scales up the P term (and the D term a little bit), which is the correct response: turning harder towards the target to overcome the added rotational drag. It still turns slowly but it becomes a bit faster and thus a bit better at following the target. 

The limitations of this demo make this happen not quite as nicely as we'd like, though:
- We made a large sudden change in the missile's handling in order to make it visually obvious for this demonstration. That makes it harder for the adaptation to find new parameters that work. In a production environment a more complex physics model would be used, which would more slowly change its handling charactistics as the missile changed speed or encountered different air pressure as it changed altitude.
- The target moves in a discontinuous way, which makes the error term discontinuous and the derivative of the error term spikey. This makes adaptation difficult because we're dividing by this derivative, further amplifying its spikey discontinuous nature. In a production environment great care must be taken to make the error term continuous: blending between different target locations and possibly even just zeroing out the error derivative if the target must pop from position to position.

In this case, the "correct" adaptation happened because of this: the target was moving quickly near to the missile which caused the error and its derivative to skyrocket. The system then kept going with the new larger P term. It was the correct response but not necessarily for the correct reason.

This demo still shows the basics of model reference adaptive control and the code can be used as the starting point for a more production-ready application.

## Running the demo

### Requirements

This sample runs on Windows, and can run on Windows 10: see the Releases tab for more info and to download the executable. I haven't tried compiling it in a long time! But at the time I used MS VC.Net 2003.

## Usage notes

- The demo starts off with a non-adaptive controller to give you a feel for what's going on. The missile is attempting to steer towards the target, and you can affect the handling of the missile by tuning the "Missile Rotational Drag," "Missile Max Angular Acceleration," and "Missile PID output scale" sliders. Notice how when you change the settings, the missile no longer is able to track the target as effectively.

- Click on the "?" buttons next to the P, I, and D sliders for a brief description of their functions.

- You can move the P, I, and D sliders to attempt to re-tune the missile's steering to match the new handling if you wish.

- You can also choose to steer the missile using the keyboard, use 8 key on the numpad to accelerate, and use 4 and 6 to steer left and right. Hopefully, you will discover that the missile is actually quite difficult to control.

- Now put the missile into adaptive control mode and watch the P, I, and D move themselves in an effort to make the missile track the target well again. If you've made a small change to the missile's handling, the sliders should re-tune themselves fairly quickly. If you've made a larger change, it may take longer, or may never even reach equilibrium.

- Click on the Reset button to reset all of the sliders to their default values.

- Click on the Pause button to pause the demo if you want to change the value of several sliders at once.

- Many tuning values, such as the round-robin timeslice, and the clamps on the P, I, and D coefficents, can be found at the top of MainDlg.cpp. The model can be found at the top of CMissile.cpp. These tuning values are present only in the code to avoid cluttering the GUI.

- Because this demo is so simple, the D term has by far the largest effect on the missile's handling; enough damping is sufficient to correct the missile's behavior no matter how its handling is set. Also, there are no external forces to induce steady-state error, and so the I term is not very useful. 
