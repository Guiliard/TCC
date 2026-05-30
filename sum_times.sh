#!/bin/bash

FILE="$1"

grep "Total Running Time" "$FILE" | \
awk '
{
    sum += $4;
    count++;
}
END {
    printf("Execuções : %d\n", count);
    printf("Tempo total: %.2f s\n", sum);
    printf("Média      : %.2f s\n", sum/count);
}'