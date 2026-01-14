//VATSU
/*
PREREQUIS: 
- 1 capteur ultason : pour mesurer la distatnce
- 1 moteur : pour controler le levie de frein
- 1 buzzer : pour le signalement sonor
- 2 bouton poussoir(ou systeme de contact): pour savoir quand le levier est a son etat normal
                                            et pouvoir reinitialiser tout le programme
- 3 realis de contatct : pour commander les broches du moteur et l'alimentation

FONCTIONNEMENT :  
  - le programme va continuelement tester si on est dans la plage ou le systeme doit 
    commencer a fonctionner, si il y est le programme vas tester et trouver la 
    plage dans la quel on se trouve, et en fonction de celle ci on peut
    lancer au conducteur un avertissement, freinner la voiture a interval definie ou
    freiner totalement et arreter la voiture.

  - apres l'execution du programme on peut appuyer sur le bouton poussoir (systeme de contact)
    pour reinitialiser le programme a zero

NOTA BENE : la position initial du relais doit etre pour le sens horlogique(abaisser le levier de frein)
            c'est seulement une fois ce relais enclacher que le moteur tourne dans le sens antihorlogique.

fait par l'Ir KATSUVA MALAMBO le 14 Janvier 2026
*/
#include <NewPing.h>                                                                     //inclure la bibliotheque pour gerer le capteur ultrason

#define zero 0                                                                           //definir la valeur zero pour ne pas avoir a se repeter
#define baud 9600                                                                       //definir la vitesse d'initialisation du moniteur serie

#define echo_pin 5                                                                      //definir le pin 5 pour la broche echo du capteur ultrason
#define trigger_pin 6                                                                  //definir le pin 6 pour la broche trigger du capteur ultrason
#define max_distance 200                                                              //definir la distance max que le capteur peut mesurer en cm

#define pin_break_state 7                                                               //definir la broche 7 pour savoir si le levier de frein est a son etat initial ou non   
#define pin_commande_reset 8                                                           //definir la broche 8 pour reinitialiser tout le systeme
#define pin_commande_buzzer 9                                                         //definir la broche 9 pour commander le buzzer

#define pin_relais_commande_un 3                                                        //definir la broche 3 comme commande du relais de la broche un du moteur
#define pin_relais_alimentation 2                                                      //definir la broche 2 comme commande du relais qui controle l'alimentation du moteur
#define pin_relais_commande_deux 4                                                    //definir la broche 4 comme commande du relais de la broche deux du moteur

#define distance_avertissement 50                                                       //definir la distance(en cm) a laquel l'utilisateur doit avoir un avertissement
#define distance_freinage_step 30                                                      //definir la distance(en cm) a laquel la voiture freinne selon un interval definie
#define distance_freinage_stop 10                                                     //definir la distance(en cm) a laquel les freins doivent etre totalement activer pour arreter de force le vehicule

NewPing capteur_ultrason(trigger_pin, echo_pin, max_distance);                          //cree l'instance du cateur ultrason

bool avertissement = false;                                                             //declarer la variable booleenne avertissement qui permet d'activer le programme d'avertissement
bool freinage_step = false;                                                            //declarer la variable booleenne freinage_step qui permet d'activer le programme de freinage par interval
bool freinage_stop = false;                                                           //declarer la variable booleennne freinage_stop qui permet d'activer le programme qui arrete le vehicule de force

bool reset_systeme = false;                                                             //declarer la variable booleenne reset_stysteme qui permet d'activer le programme qui reinitialise tout le systeme
bool active_systeme = false;                                                           //declarer la variable booleenne active_systeme tout le systeme

bool state_break_state = false;                                                         //declarer la variable booleenne state_break_state qui permet de savoir si le levier de frein est a son etat initial
bool state_commande_reset = false;                                                     //declarer la variable booleenne state_commande_reset qui permet de savoir si le bouton reset est appuyer
bool state_commande_buzzer = false;                                                   //declarer la variable booleenne state_commande_buzzer qui permet de controler le buzzer
bool state_relais_commande = false;                                                  //declarer la variable booleenne state_relais_commande qui permet de commander les relais du moteur
bool state_relais_alimentation = false;                                             //declarer la variable booleenne state_relais_alimentation qui permet de controler le relais de l'alimentation

unsigned int distance = zero;                                                           //declarer la variable entiere distance qui recoit la distance mesurer par le capteur ultrason

unsigned long present_time = zero;                                                      //declarer la variable entiere non signer present_time pour utiliser millis() et l'initialiser a zero
unsigned long previous_time_sensor = zero;                                             //declarer la variable entiere non signer previous_time_sensor pour la temporisation de la mesure par le capteur ultrason                           
unsigned long previous_time_avertissement = zero;                                     //declarer la variable entiere non signer previous_time_avertissement pour la temporisation pendant l'avertissement
unsigned long previous_time_freinage_step = zero;                                    //declarer la variable entiere non signer previous_time_freinage_step pour la temporisation pendant le freinage par interval

#define interval_mesure 50                                                              //definir l'interval en milli seconde pour mesurer a nouveau la distance
#define interval_relais_commande 1000                                                  //definir l'interval pour le programme d'avertissement et celui de freinage par interval

void setup() {  
  Serial.begin(baud);                                                                 //initialiser le moniteur serie

  pinMode(pin_break_state, INPUT);                                                    //configurer la broche pin_break_state comme entrer
  pinMode(pin_commande_reset, INPUT);                                                //configurer la broche pin_commande_reset comme entrer
  pinMode(pin_commande_buzzer, OUTPUT);                                             //congigurer la broche pin_commande_buzzer comme sortie

  pinMode(pin_relais_commande_un, OUTPUT);                                            //configurer la broche pin_relais_commande_un comme sortie
  pinMode(pin_relais_alimentation, OUTPUT);                                          //configurer la broche pin_relais_alimentation comme sortie
  pinMode(pin_relais_commande_deux, OUTPUT);                                        //configurer la broche pin_relais_commande_deux comme sortie
}

void loop() {
  present_time = millis();                                                           //affecter la valeur de millis a present_time
  state_break_state = digitalRead(pin_break_state);                                 //stocker la valeur de pin_bread_state dans la variable state_break_state
  state_commande_reset = digitalRead(pin_commande_reset);                          //stocker la valeur de pin_commande_reset dans la variable state_commande_reset

  if ((present_time - previous_time_sensor) >= interval_mesure) {                    //mettre une temporisation dans un interval egal a la valeur de interval_mesure
    distance = capteur_ultrason.ping_cm();                                          //stocker la valeur mesurer par le capteur dans la variable distance
    Serial.println(distance);                                                      //afficher la valeur mesurer par le capteru dans le moniteur serie
    previous_time_sensor = present_time;                                          //renouveller l'interval
  }

  active_systeme = ((distance <= distance_avertissement) && (distance > zero) && (distance != zero)) ? true : false; //activer la variable active_systeme si on est dans la plage donnee, la desactiver si non

  if (active_systeme) {                                                             //tester la variable active_systeme est sur true
    if ((distance <= distance_avertissement) && (distance > distance_freinage_step) && (distance != zero)) {//tester si on est dans l'interval de l'avertissement
      avertissement = true;                                                         //mettre la variable avertissement a true
      freinage_step = false;                                                       //mettre la variable freinage_step sur false
      freinage_stop = false;                                                      //mettre la variable freinage_stop sur false
    } else if ((distance <= distance_freinage_step) && (distance > distance_freinage_stop) && (distance != zero)) { //tester si on est dans l'interval du freinage par interval
      avertissement = false;                                                        //mettre la variable avertissement a false
      freinage_step = true;                                                        //mettre la variable freinage_step sur true
      freinage_stop = false;                                                      //mettre la variable freinage_stop sur false
    } else if ((distance <= distance_freinage_stop) && (distance > zero)) {         //tester sin on est dans l'interval du freinage total
      avertissement = false;                                                       //mettre la variable avertissement sur false
      freinage_step = false;                                                      //mettre la variable freinage_step sur false
      freinage_stop = true;                                                      //mettre la variable freinage_stop sur true
    }

    if (state_commande_reset) reset_systeme;                                       //si le bouton reset est presser reset le systeme

  } else {                                                                         //si le systeme n'est pas activer mettre toute les variable booleenne a false
    avertissement = false;
    freinage_step = false;
    freinage_stop = false;

    reset_systeme = false;
    state_break_state = false;
    state_commande_buzzer = false;
    state_relais_commande = false;
    state_relais_alimentation = false;
  }

  if (reset_systeme) {                                                              //tester si le bouton reset a ete appuyer
    if (!state_break_state) {                                                      //tester si le levier du frein n'est pas a son etat d'origin
      state_relais_commande = true;                                               //commander le relais du moteur pour faire tourner le moteur dans le sens antiorlogique
      state_relais_alimentation = true;                                          //commander le moteur pour allimenter le moteur
    } else {                                                                    //si le levier du frein est a son etat normal
      reset_systeme = false;                                                   //desactiver le reset du systeme
    }
    
  } else {                                                                          //si le bouton reset n'a pas ete appuyer
    if (avertissement) {                                                           //tester si la variable booleenne avertissement est sur true pour avtiver ce programme
      if ((present_time - previous_time_avertissement) >= interval_relais_commande) {//mettre une temporisation en millisecode egal a la valeur de interval_relais_commande
        state_relais_alimentation = false;                                        //desactiver l'alimentation du moteur
        state_commande_buzzer = !state_commande_buzzer;                          //allumer et etteindre le buzzer dans l'interval definie
        previous_time_avertissement = present_time;                             //renouveller le temps precedent
      }
    }

    if (freinage_step) {                                                           //tester si la variable booleenne freinage_step est sur true pour activer ce programme
      if ((present_time - previous_time_freinage_step) >= interval_relais_commande) {//mettre une temporisation en millicesonde egal a la valeur de interval_relais_commande
        state_relais_alimentation = true;                                         //activer l'alimentation du moteur
        state_commande_buzzer = !state_commande_buzzer;                          //allumer et etteindre le buzzer dans l'interval definie
        state_relais_commande = !state_relais_commande;                         //faire tourner le moteur dans le sens horlogique et antihorlogique dans l'interval definie
        previous_time_freinage_step = present_time;                            //renouveller le temps precedent
      }
    }

    if (freinage_stop) {                                                         //tester si la variable booleenne frinage_stop est sur true pour activer ce programme
      state_commande_buzzer = true;                                             //allumer continuellement le buzzer
      state_relais_commande = false;                                           //faire tourner le moteur dans le sens horlogique
      state_relais_alimentation = true;                                       //activer l'alimentation du moteur
    }
  }

  digitalWrite(pin_commande_buzzer, state_commande_buzzer);                      //commander le buzzer en fonction de la variable state_commande_buzzer
  digitalWrite(pin_relais_commande_un, state_relais_commande);                  //commander le relais de la premiere broche du moteur en fonction de la variable state_relais_commande
  digitalWrite(pin_relais_commande_deux, state_relais_commande);               //commander le relais de la deuxieme broche du moteur en fonction de la variable state_relais_commande
  digitalWrite(pin_relais_alimentation, state_relais_alimentation);           //commander le relais de l'alimentation du moteur en fonction de la variable state_relais_alimentation
}
