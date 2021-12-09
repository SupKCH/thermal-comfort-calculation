function output_data_table = day_light_saving_shift(data_table)
    toShift = data_table.timestamp >= datetime(2021, 11, 8, 0, 0, 0);
    data_table.timestamp(toShift,:) = data_table.timestamp(toShift,:) + 1/24;
    output_data_table = data_table;
end