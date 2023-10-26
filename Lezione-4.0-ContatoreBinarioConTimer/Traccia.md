Si implementi il contatore binario che incrementi di uno ad ogni scadenza del timer
Se lo si implementa mediante un'attesa (si attende che arrivi la interrupt del timer e si incrementa) si impegna il processore solo per quel compito
Se invece si effettua una serie di operazioni e all'inzio di ogni ciclo si controlla se è arrivato l'update dal timer si rischia di non cogliere l'evento quando arriva(Soluzione implementata in questo esempio)
