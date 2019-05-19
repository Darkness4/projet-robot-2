#ifndef _MOTEUR_H_
#define _MOTEUR_H_

void CommandeMoteur(int percent);
void Tourner(int percent, char direction);
void Avancer(int percent);
void Calibration(void);
void Decelerer(void);

#endif