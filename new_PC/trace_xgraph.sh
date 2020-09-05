#!/bin/bash
if [ -f "./hello" ];
then
#	launching trace with xgraph (./hello)
else
	echo "*** Aucun fichier .xg disponible pour tracer ***";
	echo "*** Lancez une Acquisition pour en créer un. ***";
fi
