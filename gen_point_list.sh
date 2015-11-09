#!/bin/bash
# This generates a big list of vectors for use in convex_hull

if [ ! -n "$1" ]; then
	echo "Usage: gen_point_list.sh <number of points>"
	exit 1
fi

echo "#ifndef CH_DATA_POINTS"
echo "#define CH_DATA_POINTS"
echo "struct ch_point { float x; float y; };"
echo "struct ch_point ch_point_list[] = {"

for (( i=0; i<$1; i++)); do
	echo "{${RANDOM}.0f, ${RANDOM}.0f },"
done

echo "};"
echo "int ch_point_list_size = sizeof(ch_point_list) / sizeof(struct ch_point);"
echo "#endif"
