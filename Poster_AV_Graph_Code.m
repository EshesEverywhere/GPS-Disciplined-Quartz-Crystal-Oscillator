y=Allan_Variance_Full_Measurements_Table.(3);
t=Allan_Variance_Full_Measurements_Table.(6);
semilogx(t,y,'-o','MarkerFaceColor',[0 0.4470 0.7410],'MarkerSize',5,'LineWidth',1.5)
title('Allan Deviation vs. Tau (s)');
xlabel('Tau (s)');
ylabel('Allan Deviation');
