./create_template

for ((i = 0; i < 100; i++));
    do
        echo "#!/bin/bash" > script_c2_k12_p0.80_q0.04_$i.sh
        echo "./main 2 12 0.80 0.04 c2_k12_p0.80_q0.04_$i" >> script_c2_k12_p0.80_q0.04_$i.sh
        echo " " >> script_c2_k12_p0.80_q0.04_$i.sh
        chmod a+x script_c2_k12_p0.80_q0.04_$i.sh
        qsub -cwd script_c2_k12_p0.80_q0.04_$i.sh

        sleep 1

        echo "#!/bin/bash" > script_c4_k12_p0.78_q0.06_$i.sh
        echo "./main 4 12 0.78 0.06 c4_k12_p0.78_q0.06_$i" >> script_c4_k12_p0.78_q0.06_$i.sh
        echo " " >> script_c4_k12_p0.78_q0.06_$i.sh
        chmod a+x script_c4_k12_p0.78_q0.06_$i.sh
        qsub -cwd script_c4_k12_p0.78_q0.06_$i.sh

        sleep 1

        echo "#!/bin/bash" > script_c4_k12_p0.80_q0.04_$i.sh
        echo "./main 4 12 0.80 0.04 c4_k12_p0.80_q0.04_$i" >> script_c4_k12_p0.80_q0.04_$i.sh
        echo " " >> script_c4_k12_p0.80_q0.04_$i.sh
        chmod a+x script_c4_k12_p0.80_q0.04_$i.sh
        qsub -cwd script_c4_k12_p0.80_q0.04_$i.sh

        sleep 1

        echo "#!/bin/bash" > script_c4_k12_p0.82_q0.02_$i.sh
        echo "./main 4 12 0.82 0.02 c4_k12_p0.82_q0.02_$i" >> script_c4_k12_p0.82_q0.02_$i.sh
        echo " " >> script_c4_k12_p0.82_q0.02_$i.sh
        chmod a+x script_c4_k12_p0.82_q0.02_$i.sh
        qsub -cwd script_c4_k12_p0.82_q0.02_$i.sh

        sleep 1

        echo "#!/bin/bash" > script_c4_k18_p0.80_q0.04_$i.sh 
        echo "./main 4 18 0.80 0.04 c4_k18_p0.80_q0.04_$i" >> script_c4_k18_p0.80_q0.04_$i.sh
        echo " " >> script_c4_k18_p0.80_q0.04_$i.sh
        chmod a+x script_c4_k18_p0.80_q0.04_$i.sh
        qsub -cwd script_c4_k18_p0.80_q0.04_$i.sh

	sleep 1

	echo "#!/bin/bash" > script_c4_k6_p0.80_q0.04_$i.sh
        echo "./main 4 6 0.80 0.04 c4_k6_p0.80_q0.04_$i" >> script_c4_k6_p0.80_q0.04_$i.sh
        echo " " >> script_c4_k6_p0.80_q0.04_$i.sh
        chmod a+x script_c4_k6_p0.80_q0.04_$i.sh
        qsub -cwd script_c4_k6_p0.80_q0.04_$i.sh

        sleep 1

        echo "#!/bin/bash" > script_c6_k12_p0.80_q0.04_$i.sh
        echo "./main 6 12 0.80 0.04 c6_k12_p0.80_q0.04_$i" >> script_c6_k12_p0.80_q0.04_$i.sh
        echo " " >> script_c6_k12_p0.80_q0.04_$i.sh
        chmod a+x script_c6_k12_p0.80_q0.04_$i.sh
        qsub -cwd script_c6_k12_p0.80_q0.04_$i.sh

        sleep 1

    done
