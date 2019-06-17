MAX_A=0
MAX_B=20

for ((a=0;a<=MAX_A;a++));
do
	for ((b=0;b<=MAX_B;b++));
	do
		echo "10.0.$a.$b" >> nodes
	done
done
