import exodusii

exo_file = exodusii.exodusii_file('binder_in.e')
sidesets = exo_file.get_side_set_names()
sideset_ids = exo_file.get_side_set_ids()

#print(sideset_ids)

pair_side_count = 0
non_pair_count = 0
test = 0

total_count = len(sidesets)

pairing_list = []
pairing_list_ids = []


for index, side in enumerate(sidesets):
    if side[0] == 'G':
        pairing_list.append(side)
        pairing_list_ids.append(sideset_ids[index])
        pair_side_count += 1
    else:
        non_pair_count += 1
        continue

# initial = 'G_2D_'
# final = 'g_2d_'

boundary_pairs = []

for side in pairing_list:
    side.replace('G_2D_','g_2d_')
    split = side.split("_")
    primary = int(split[2])
    secondary = int(split[5])
    boundary_pairs.append([primary, secondary])

#print(pairing_list)

pair_count = int(pair_side_count/2)
count = 0
sorted_boundaries = []
primary_set = []
secondary_set = []
repeated = []
final_primary_sideset = []
final_output = []

for index, row in enumerate(boundary_pairs):
    primary = row[0]
    secondary = row[1]
    for index_2, row_2 in enumerate(boundary_pairs):
        if row_2[0] == secondary and row_2[1] == primary:
            temp = [index, index_2]
            temp_rev = [index_2, index]
            if temp in sorted_boundaries:
                continue
            elif temp_rev in sorted_boundaries:
                continue
            else:
                sorted_boundaries.append(temp)
                primary_set.append(index)
                final_primary_sideset.append(str(pairing_list[index]).lower())
                final_output.append(int(pairing_list_ids[index]))
                # count += 1
    if count == pair_count:
        break

#print(final_primary_sideset)
print(*final_output)
# test_index = 245
# test1, test2 = sorted_boundaries[test_index]
# primary = pairing_list[test1]
# secondary = pairing_list[test2]
# print(final_output)

# for item in final_output:
#     print(item)

######################################################################################################################################


# play = pairing_list[0]
# # print(len(play))
# print(play)

# len_primary = 5
# len_secondary = 6

# primary_find = 'G_2D_'
# secondary_find = '_G_2D_'


# print(pair)
# print(non_pair)
# print(test)
# print(sidesets)
# print(test)

# boundary_pairs = np.zeros((pair_count,2))
# boundary_pairs = boundary_pairs.tolist()
# print(len(boundary_pairs[0]))

#print(boundary_pairs)