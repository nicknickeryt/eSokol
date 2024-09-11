  uint8_t pulse_count = 0; 				// jakis licznik

  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL); 	// start trybu enkodera PA8 PA9 (prescaler = 255, input filter (obydwa = 15)
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 		// start PWM czyli kontroli ledow
  while(1) {
	pulse_count = TIM1->CNT;			// wziecie do pulse_count wartosci z TIM1. 
							// PWM mamy od 0 do 255, ale pulse_count bedzie 0 po 255, bo jest uint8_t, wiec gites :))
	htim2.Instance->CCR2 = pulse_count;		// ustawienie duty cycle
	
	
	
// Wersja ulepszona, bardziej liniowa:

  uint8_t pulse_count = 0;

  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  while(1) {
		pulse_count = pow(TIM1->CNT, 2);

		htim2.Instance->CCR2 = pulse_count;
		
// Używamy funkcji kwadratowej, bo ona dla małych argumentów mało je zwiększa, ale im większe, tym bardziej je zwiększa.
// To jest potrzebne, bo dla malych zmian w duty cycle zmiana w jasnosci diody jest duza, ale dla duzych duty cycle - zmian w jasnosci mala.
// Uwaga. Jako ze funkcja y=x^2 osiagnie 255 (maksimum PWM) w punkcie ~16 (15.9), to niech prescaler liczy od 0 do 16.
