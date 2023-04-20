import csv

with open('load_test_stats.csv', newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    last_row = None
    for row in reader:
        last_row = row

total_request_count = int(last_row['Request Count'])
average_response_time = float(last_row['Average Response Time'])
total_requests_per_second = float(last_row['Requests/s'])
average_max_response_time = int(last_row['Max Response Time'])

print("=================================================")
print(f"Total Request Count: {total_request_count}")
print(f"Average Response Time: {average_response_time:.2f} ms")
print(f"Average Maximum Per Second: {average_max_response_time} ms")
print(f"Total Requests Per Second: {total_requests_per_second:.2f}")
