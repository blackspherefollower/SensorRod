# SensorRod - A digitizer for internal textures

##Overview

tl;dr: This is a device for sensing the textures of Fleshlights for people without penises.

The idea come to me after a friend described the feeling of penis envy she'd had whilst browsing Fleshlights at an adult store: the daemons in my head just fire off as soon as they pick up on a problem they think they can solve (it can get very irritating, as it's not something I can control well). My though process went like this: If the feeling of moving against the internal textures could be digitized, the signal could be output in a way that maps the experience. I use the term map here because I highly doubt it's possible to produce the exact same feeling stimulation, and certainly no way of even knowing if that the sensation is the same.

The breakdown of my thoughts will be a common theme here, since I'm basically typing them down interleaved with what I learnt whilst researching and implementing the prototype. Much of this happened during the SexTechHack 2017 event (http://goldsmiths.tech/sex).

###The problem of actual sensing internal textures

The first couple of things that came to mind were pressure or capacitive sensors: neither suitable due to lack of resultion. Capacitive sensors would also require conductive materials, which Fleshlights and other similar products are rarely made of. Pressure sensors would struggle with the softness of the textures too.

Niether of these are things a human senses either; sure we feel pressure, but the pressure of a Fleshlight around us isn't that stimulating, and even changing pressures alone is unlikley to provoke too much response.

What we do react to is friction! Fleshlights and sililar products are often refered to as strokers for the reason that we feel the movement against our skin: friction.

Now, how do we sense that? Sure accelerometers and gyroscopes sense movement, but they're realative to the earth. How about mice: they detect movement between their base and the surface they're ontop of. Better still, defects in the material of the surface often cause mice to jump, something that might be quite desirable when trying to sense texture.

As it happens, I had a box of old PS2 mice (the ones with the round DIN connectors, not ones for use with Sony's console). So, after opening a few I found the mot common optical sensor (there were at least 3 different sensors used in just 6 identically cased mice) and found this blog post:

https://lukelectro.wordpress.com/2013/11/15/quick-and-dirty-arduino-optical-mouse-sensor-experiment-a2620-chip/

Which led me to this blog post:

https://conorpeterson.wordpress.com/2010/06/04/optical-mouse-hacking-part-1/

Which after getting irritated at trying to follow a YouTube video made me crack and dig up this datasheet:

http://mjolnir.lille.inria.fr/turbotouch/lagmeter/datasheet_ADNS2610.pdf



On the plus side, I how had 2 versions of Arduino code that supposedly worked with the mouse ICs I had. So, I started by trying to reproduce the examples in the blog posts: they were primarily aimed at using the mouse ICs as low resolution video devices, but I could not get that to work: just kept getting an odd interference pattern. But I did start getting reasonable X/Y delta movement readings.

So I modified the code so that I could get results from 4 mice all sharing the same clock wire (you'll find that in this repo). As soon as the serial connection looks good, the Arduino begins reading the deltas and sending them over that connection in ASCII.

### Reading the input

I wasn't about to reinvent the wheel here: I wanted to reuse as much software as I could. So I implemented a device driver for https://buttplug.io that found the device on whatever serial port it was connected, and started reading the movement data. I had to extend the message schema to allow for the type of message I was passing, but since input device support is still in development in general, this was something I'd gotten quite used to.

I tweaked the example client too, adding a switch for enabling movement data. Connecting to the Buttplug server I could scan and detect the device, request movement data and then see that data arrive at the client.

The hacky driver code can be found here: https://github.com/blackspherefollower/buttplug-csharp/tree/sextechhack

It wouldn't be too much of a stretch from here to convert that data into something that output devices could accept.

### And then it all fell apart...

The whole point of the project was to have these sensors pointed outwards from inside a clear acrylic tube that can be inserted within a Fleshlight. But my tube's inner diameter was significantly less than the mouse PCBs.

Checking the datasheet, I could see which components on the PCB were needed for the optical flow sensor and which were for the other mouse functions (buttons and PS2 interfacing). Now I knew which components I could remove and where I could cut down the PCB.

Or so I thought...

What I was left with was a non-functioning brutalized mouse PCB soldered to an Arduino that was unable to report anything useful. Not even the guide LED on mouse was coming on now.

It got worse as I was then unable to reproduce my previous success of reading from the mouse sensors with any other Arduino I happened to have.

I accepted my defeat and presented the concept and a shorter version of the tale so far at the show and tell and the end of the hackathon.

## I'm not done yet!

The really interesting thing was the interest I got from other attendees of the hackathon. The idea has legs, and optical sensing seems like an effective means of digitizing the data. So I'm not going to stop until I have a fully functional prototype.

Watch this space!