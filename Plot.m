clear all;
[Time,X,Y,ANG] = importfile('test.txt');
h = figure;
hh2 = plot([0, X(1, 1)], [0,-Y(1, 1)], ...
'.-', 'MarkerSize', 20, 'LineWidth', 2);
axis equal
axis([-2 2 -2 2]);

n_elements = length(X);

filename = 'Pendulum.gif';
for i = 1:200:n_elements
   set(hh2(1), 'XData', [0, X(i, 1)]  , 'YData', [0, -Y(i, 1)]);
   drawnow;
   
   % Capture the plot as an image 
   frame = getframe(h); 
   im = frame2im(frame); 
   [imind,cm] = rgb2ind(im,256); 
   % Write to the GIF File 
%    if i == 1 
%        imwrite(imind,cm,filename,'gif', 'Loopcount',inf); 
%    else 
%        imwrite(imind,cm,filename,'gif','WriteMode','append'); 
%    end 
end