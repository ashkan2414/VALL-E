function fig = plot_heat_map(spa_results, log_scale_flag)
    fig = figure;
    t = linspace(0,2*pi);
    plot(cos(t),sin(t),'k--');
    hold on;
    if log_scale_flag
        scatter(real(spa_results.params.ps),imag(spa_results.params.ps),50,log(abs(spa_results.w_coeffs)),'filled');
        scatter(real(spa_results.params.qs(1:spa_results.params.nhat)),imag(spa_results.params.qs(1:spa_results.params.nhat)),50,log(abs(spa_results.xhat_coeffs)),'filled');
    else
        scatter(real(spa_results.params.ps),imag(spa_results.params.ps),50,abs(spa_results.w_coeffs),'filled');
        scatter(real(spa_results.params.qs(1:spa_results.params.nhat)),imag(spa_results.params.qs(1:spa_results.params.nhat)),50,abs(spa_results.xhat_coeffs),'filled');
    end
    hold off;
    colormap(jet);
    colorbar;
end