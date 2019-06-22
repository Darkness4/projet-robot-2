/**
 * @file moteur.h
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Commandes moteur
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#ifndef _MOTEUR_H_
#define _MOTEUR_H_

void CommandeMoteur(int percent);
void Tourner(int percent, char direction);
void Avancer(int percent);
void Calibration(void);
void StopMoteur(void);

#endif