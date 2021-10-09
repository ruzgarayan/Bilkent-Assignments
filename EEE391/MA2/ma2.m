A=imread('couple.bmp');
J=mat2gray(A, [0 255]);
imshow(J);

m = 11;
mOver2 = (m - 1)/2;

c = 0.2;
noise = rand(512,512);
noise = noise - 0.5;
noise = noise * c;
J = J + noise;

J = [zeros(512, mOver2) , J, zeros(512, mOver2)];
res = zeros(512, 512);
for i = 1:512
    for j = 1:512
        res(i,j) = 0;
        for k = 0:(m - 1)
            res(i,j) = res(i,j) + J(i, j + k); 
        end
    end
end
res = res / m;

omegaHat = linspace(-pi, pi, 1000);
freqResponse = zeros(1000);

for a = 1:1000
    freqResponse(a) = 0;
    for b = -mOver2:mOver2
        freqResponse(a) = freqResponse(a) + exp(-1.0i * omegaHat(a) * b);
    end
    freqResponse(a) = freqResponse(a) / m;
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