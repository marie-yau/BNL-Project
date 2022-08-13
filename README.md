# Model of Charge Distribution in Electron Camera
The Soft Inelastic X-ray Scattering (SIX) beamline at the National Synchrotron
Light Source II located at the Brookhaven National Laboratory uses
x-ray to study the composition of a material. It is done by letting the x-ray
scatter over the material. The x-ray is then detected by an electron camera
(EMCCD) that is composed of a pixelated grid. By measuring the charge accumulated
in each pixel, the camera produces an image showing the proportion of
the x-ray that was scattered over the material, enabling us to find out what its
structure is. Using this method of x-ray scattering, it is possible to achieve a
very high image accuracy because we can deduce the position of the x-ray with
accuracy better than its pixel pitch. The goal of my project was to develop a
program that calculates the exact position of the x-ray given an image. When we
receive a charge distribution for an unknown original x-ray coordinate, we can
fit two-dimensional Gaussian model to our distribution using the least squares
method. We have estimated the model accuracy by precomputing charge values
for many different x-ray positions on the detector using a mathematical formula.
Since the x-ray generates a charge on the pixel it hits and its neighboring pixels,
we generate a charge distribution over detector’s pixels for each x-ray position.
As a result of my internship, I have developed a program that enables other
scientists to study structures of various materials more accurately. During the
process, I have improved my programming skills and learned to work with many
optimization packages.