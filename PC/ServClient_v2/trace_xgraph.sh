#!/bin/bash
if [ -f "./hello" ];
then
	./hello
else
	echo "*** Aucun fichier .xg disponible pour tracer ***";
	echo "*** Lancez une Acquisition pour en créer un. ***";
fi
