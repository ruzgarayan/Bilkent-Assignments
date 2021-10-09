A=imread('couple.bmp');
J=mat2gray(A, [0 255]);
imshow(J);

J = [zeros(512, 1) , J];
res = J(:,2:513) - J(:,1:512);

omegaHat = linspace(-pi, pi, 1000);
freqResponse = zeros(1000);

for a = 1:1000
    freqResponse(a) = 1 - exp(-1.0i * omegaHat(a) * 1);
end

freqResponseMagnitude = abs(freqResponse);
plot(omegaHat, freqResponseMagnitude);
title('Plot of Frequency Response Magnitudes Between -\pi and \pi');
xlabel('-\pi < frequency < \pi');
ylabel('Magnitude of the Frequency Response');
xlim([-pi pi]);
set(gca,'XTick',[-pi -3*pi/4 -pi/2 -pi/4 0 pi/4 pi/2 3*pi/4 pi]);
set(gca,'XTickLabel',{'-\pi', '-3\pi/4', '-\pi/2', '-\pi/4', '0', '\pi/4', '\pi/2', '3\pi/4', '\pi'});

input('ENTER for the resulting image.');

imshow(res);
imwrite(res, 'sonuc.bmp');