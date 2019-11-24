awk '{if($8 != 54 && $8 != 66 && $8!=74) print $2, $8}' xia_flow1_result.txt > xia_pdcp_filtered.txt
awk -F',' '{print $2, $17, $18}' 1c.csv  | awk '{if($3+$4+22!=1412 && $3+$4+22!=54 && $3+$4+22!=66 && $3+$4+22!=74) print $2, $3 + $4 + 22}' > 1c_anal.csv
