clc; clear; close all;

Time = [
    29498
29799
30100
30401
30702
31003
31304
31605
31906
32207
32508
32809
33110
33411
33712
34013
34314
34615
34916
35217
35518
35819
36120
36421
36722
37023
37324
37625
37926
38227
38528
38829
39130
39431
39732
40033
40334
40635
40936
41237
41538
41839
42140
42441
42742
43344
43645
43946
44247
44548
44849
45150
45451
45752
46053
46354
46655
46956
47257
47558
47859
48160
48461
48762
49063
49364
49665
49966
50267
50568
50869
51170
51471
51772
52073
52374
52675
52976
53277
53578
53879
54180
54481
54782
55083
55384
55685
55986
56287
56588
56889
57190
57491
57792
58093
58394
58695
58996
59297
59598
59899
60200
60501
60802
61103
61404
61705
62006
62307
62608
62909
63210
63511
63812
64113
64414
64715
65016
65317
65618
65919
66220
66521
66822
67123
67424
67725
68026
68327
68628
68929
69230
69531
69832
70133
70434
70735
71036
71337
71638
71939
72240
72541
72842
73143
73444
73745
74046
74347
74648
74949
75250
75551
75852
76153
76454];

Velocity = [0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.00
0.16
0.16
0.16
7.74
12.94
16.73
18.35
18.78
19.36
20.63
23.27
23.96
24.16
24.16
24.26
24.16
24.16
24.26
24.37
23.96
23.75
23.46
23.46
22.98
22.80
22.61
22.43
22.17
21.58
21.33
21.10
20.94
20.71
20.48
20.34
19.98
19.77
19.49
18.91
17.83
16.48
14.84
12.76
12.76
11.35
11.35
8.11
8.11
8.11
8.11
8.11
8.11
8.11
8.11
8.11
0.00
0.00
0.00
0.00
0.00
0.00
0.00
1.19
1.19
10.81
14.03
18.00
22.43
24.06
25.47
25.24
25.02
24.26
23.96
24.16
24.26
24.06
24.37
24.06
24.47
24.26
24.06
23.96
23.46
23.36
22.52
22.52
22.52
22.52
22.52
3.71
3.71
3.71
5.52
5.52
5.52
7.29
13.08
13.08
15.79
16.58
17.61
20.12
21.02
21.66
21.66
21.66
22.00
22.98
23.17
23.27
23.36
22.98
23.75
23.56
23.36
22.98
22.80
22.35
22.17
22.00
21.74
21.58
21.25
21.10
20.71
20.41
20.26
19.98
19.77
19.49
19.29
19.03
18.78
18.54
18.30
18.00
17.83
17.61];

Acceleration = [0; diff(Velocity)];

    figure(1); clf;

    plot(Time, Velocity); grid on;

    figure(2);
    subplot(3,1,1);
        plot(Time(Time>=35000 & Time <=40000), Velocity(Time>=35000 & Time <=40000)); grid on;
        title("Próba 1 - samonapędzanie z pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,2);
        plot(Time(Time>=52800 & Time <=56500), Velocity(Time>=52800 & Time <=56500)); grid on;
        title("Próba 2 - samonapędzanie z dużą pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,3);
        plot(Time(Time>=62300 & Time <=69300), Velocity(Time>=62300 & Time <=69300)); grid on;
        title("Próba 3 - samonapędzanie bez pomocy (ale nie od 0)");
        xlabel("T [ms]"); ylabel("V [km/h]");


    figure(3);
    subplot(3,1,1);
        plot(Time(Time>=35000 & Time <=40000), Acceleration(Time>=35000 & Time <=40000)); grid on;
        title("Próba 1 - samonapędzanie z pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,2);
        plot(Time(Time>=52800 & Time <=56500), Acceleration(Time>=52800 & Time <=56500)); grid on;
        title("Próba 2 - samonapędzanie z dużą pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,3);
        plot(Time(Time>=62300 & Time <=69300), Acceleration(Time>=62300 & Time <=69300)); grid on;
        title("Próba 3 - samonapędzanie bez pomocy (ale nie od 0)");
        xlabel("T [ms]"); ylabel("V [km/h]");

figure(4);
    subplot(3,1,1);
        semilogy(Time(Time>=35000 & Time <=40000), Velocity(Time>=35000 & Time <=40000)); grid on;
        title("Próba 1 - samonapędzanie z pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,2);
        semilogy(Time(Time>=52800 & Time <=56500), Velocity(Time>=52800 & Time <=56500)); grid on;
        title("Próba 2 - samonapędzanie z dużą pomocą");
        xlabel("T [ms]"); ylabel("V [km/h]");
    subplot(3,1,3);
        semilogy(Time(Time>=62300 & Time <=69300), Velocity(Time>=62300 & Time <=69300)); grid on;
        title("Próba 3 - samonapędzanie bez pomocy (ale nie od 0)");
        xlabel("T [ms]"); ylabel("V [km/h]");