clear all;
data = importdata('../data.csv');
n_cols = size(data);
n_body = (n_cols(2)-1) / 3;
h = figure;
X = [0];
Y = [0];
Z = [0];
for i = 1:((n_cols(2)-1) / 3)
    X = [X, data(1, (i - 1) * 3 + 2)];
    Y = [Y, data(1, (i - 1) * 3 + 3)];
    Z = [Z, data(1, (i - 1) * 3 + 4)];
end
hh2 = plot3(X, Y, -Z, '.-', 'MarkerSize', 20, 'LineWidth', 2);
axis equal;
axis([-2*n_body 2*n_body -2*n_body 2*n_body -2*n_body 2*n_body]);
view(0, 0);
grid on;
n_elements = n_cols(1);
pause(5);
for i = 1:30:n_elements
    X = [0];
    Y = [0];
    Z = [0];
    for j = 1:((n_cols(2)-1) / 3)
        X = [X, data(i, (j - 1) * 3 + 2)];
        Y = [Y, data(i, (j - 1) * 3 + 3)];
        Z = [Z, data(i, (j - 1) * 3 + 4)];
    end
    set(hh2(1), 'XData', X , 'YData', Y, 'ZData', -Z);
    drawnow;
end