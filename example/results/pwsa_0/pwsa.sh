
./scripts/run_scen.py -scen myscen/sc1/AcrosstheCape.map-25.scen -algo "wPWSA*" -w 1.0,1.2,1.5,2.0 -proc 1,4,8,16,32 -exptime 0.0 -runs 6 -attempts 3 -output results/pwsa_0/AcrosstheCape.txt && ./scripts/run_scen.py -scen myscen/sc1/Expedition.map-25.scen -algo "wPWSA*" -w 1.0,1.2,1.5,2.0 -proc 1,4,8,16,32 -exptime 0.0 -runs 6 -attempts 3 -output results/pwsa_0/Expedition.txt && ./scripts/run_scen.py -scen myscen/mazes/maze512-4-0.map-25.scen -algo "wPWSA*" -w 1.0,1.2,1.5,2.0 -proc 1,4,8,16,32 -exptime 0.0 -runs 6 -attempts 3 -output results/pwsa_0/maze512-4-0.txt && ./scripts/run_scen.py -scen myscen/random/random512-25-0.map-25.scen -algo "wPWSA*" -w 1.0,1.05,1.1,1.15 -proc 1,4,8,16,32 -exptime 0.0 -runs 6 -attempts 3 -output results/pwsa_0/random512-25-0.txt
