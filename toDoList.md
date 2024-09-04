# TO DO LIST

## CLIENT
- Gestion des erreurs
    - [ ] 🔴 Leaks
    - [ ] 🟡 Messages d'erreurs en cas de paramètres incohérents
    - [ ] 🔴 Quitter proprement et libérer les ressources du client en cas d'erreur, il y a un segfault
    - [ ] 🟢 Empêcher les doublons de clients
- À faire
    - [ ] 🟡 Gérer le mode opérateur du serveur
    - [ ] 🟢 Gérer l'envoi partiel de l'input (xxxx ctrl + D) voir NETCAT

## IRC
- Gestion des erreurs
    - [x] 🔴 Leaks
    - [x] 🔴 Quitter proprement le programme : Leaks, FDs...

## MAIN
- Gestion des erreurs
    - [x] 🔴 Signaux : ctrl + C ?; ctrl + \ ?
    - [ ] 🔴 Segfault lors que j'essaie de me connecter et que je n'y arrive pas puis je fais un ctrl + c => comprendre pq


=========================================================================
# TEST
## CLIENT
1. PASS
    - Tester different mots de pass
    - Voir si l'on ferme bien le client
    - Voir si on lui envoie bien les messages d'erreur
2. NICK
    A la premiere connection le NICK devient un uniqID
    Le NICK name peut etre changer.
    Lors de la creation d'un nouveau client voir si le nouveau NICK ne match
    pas avec les uniqID et NICKNAME existant
    - PAS DE DOUBLON
3. USER
    Voir si dans IRSSI on peut changer les elements du USER a la connection


