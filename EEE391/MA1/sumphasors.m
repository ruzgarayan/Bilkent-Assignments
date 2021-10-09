%Input from user
omega_0 = input('Enter omega_0: ');

A = [0, 0, 0];
phi_degrees = [0, 0, 0];

A(1) = input('Enter A_1: ');
A(2) = input('Enter A_2: ');
A(3) = input('Enter A_3: ');
phi_degrees(1) = input('Enter phi_1: ');
phi_degrees(2) = input('Enter phi_2: ');
phi_degrees(3) = input('Enter phi_3: ');

%Convert into principal interval
for a = 1:3
    while phi_degrees(a) > 180
        phi_degrees(a) = phi_degrees(a) - 360;
    end
    while phi_degrees(a) <= -180
        phi_degrees(a) = phi_degrees(a) + 360;
    end
end

%Convert into radians
phi_radians = pi * phi_degrees / 180;

phasors = A .* exp(1i*phi_radians);

sum_of_phasors = sum(phasors);
real_part_sum = real(sum_of_phasors);
imag_part_sum = imag(sum_of_phasors);

sum_A = sqrt(real_part_sum^2 + imag_part_sum^2);
sum_phi_radians = atan2(imag_part_sum, real_part_sum);
sum_phi_degrees = sum_phi_radians * 180 / pi;

fprintf('\nResulting A = %.4f\n', sum_A);
fprintf('Resulting phi in degrees = %.4f\n', sum_phi_degrees);
if sum_phi_radians >= 0
    fprintf('\nResulting sinusodial \n\t x(t)= %.2f cos(%dt+%.2f)\n', sum_A, omega_0, sum_phi_radians);
else
    fprintf('\nResulting sinusodial \n\t x(t)= %.2f cos(%dt-%.2f)\n', sum_A, omega_0, abs(sum_phi_radians));    
end

figure('Name', 'Part (b) - IV')
quiver(zeros(3,1),zeros(3,1),real(phasors.'),imag(phasors.'), 0, 'linewidth', 1.1);
hold on
quiver(0, 0, real_part_sum, imag_part_sum, 0, 'linewidth', 1);
axis equal
ylim([-30, 30]);
xlim([-30, 30]);
title('Phasors starting from the origin');
xlabel('x (Real Part)'), ylabel('y (Imaginary Part)');

startingPoints = [0 ; phasors(1); phasors(1) + phasors(2)];
endingPoints = [phasors(1) ; phasors(2); phasors(3)];

figure('Name', 'Part (b) - V')
quiver(real(startingPoints),imag(startingPoints),real(endingPoints),imag(endingPoints), 0, 'linewidth', 1.1);
hold on
quiver(0, 0, real_part_sum, imag_part_sum, 0, 'linewidth', 1);
axis equal
ylim([-30, 30]);
xlim([-30, 30]);
title('Phasors end-to-end');
xlabel('x (Real Part)'), ylabel('y (Imaginary Part)');