function [r,e] = AngleMethod(m)
%ANGLEMETHOD Summary of this function goes here
%
%   Consider an underwater object of known depth D.  
%   Now, choose a point above the surface of the water of known location 
%   with respect to some origin.  The height of this point above the surface 
%   is h.  The line between this measurement point and the perceived object 
%   forms an angle theta1 with the normal to the surface.  This line 
%   represents a light ray coming from the object.  At the interface 
%   between the air and water, this angle makes a discontinuous jump 
%   described by Snell's Law.  The angle between the normal to the surface 
%   and the underwater light ray is theta2.  Since the depth is 
%   known, the intersection between the light ray and the horizontal line 
%   at depth D uniquely determines the horizontal distance from the 
%   measurement point to the underwater object.  Since the height of the 
%   measurement, depth of the object, and angle to the normal are only 
%   known to a certain precision, the error in the horizontal distance 
%   depends on how well each of these parameters is known.
%
%   m - measurement object
%       See Measurement.m
%
%   r - result structure
%       r.xobj    - estimated x coordinate of Object
%       r.sigxobj - error in r.x
%       r.yobj    - estimated y coordinate of Object
%       r.sigyobj - error in r.y
%
%   e - error structure (optional) - shows components of error
%
%
%   Note: All Angles MUST be in Radians

n1 = 1;
n2 = 1.333;

r.L = m.h*tan(m.theta) + m.D*tan(asin((n1/n2)*sin(m.theta)));

e.dLdD = tan(m.theta);
e.dLdh = tan(asin((n1/n2)*sin(m.theta)));
e.dLdtheta1 = m.h*(sec(m.theta))^2 + m.D*((n1/n2)*cos(m.theta))/((1-(((n1/n2)^2)*((sin(m.theta))^2)))^(3/2));

r.sigL = sqrt( (e.dLdh*m.sigh)^2 + (e.dLdD*m.sigD)^2 + (e.dLdtheta1*m.sigtheta)^2 );

r.xobj = m.x + r.L*sin(m.phi);
r.yobj = m.y + r.L*cos(m.phi);

e.dxobjdx = 1;
e.dxobjdL = sin(m.phi);
e.dxobjdphi = r.L*cos(m.phi);

e.dyobjdy = 1;
e.dyobjdL = cos(m.phi);
e.dyobjdphi = -r.L*sin(m.phi);

r.sigxobj = sqrt( (e.dxobjdx*m.sigx)^2 + (e.dxobjdL*r.sigL)^2 + (e.dxobjdphi*m.sigphi)^2 );
r.sigyobj = sqrt( (e.dyobjdy*m.sigy)^2 + (e.dyobjdL*r.sigL)^2 + (e.dyobjdphi*m.sigphi)^2 );

end
