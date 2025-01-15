<style>
table, th, td {
    text-align: left
}
</style>

# Utility Classes Documentation

This is the utilities documentation and an overview of its classes ([/src/utils](../utils)). These Classes are 
additional utilities used in the application. They are included into the application as a module ([RIOT-OS Documentation](https://doc.riot-os.org/creating-modules.html)).

## Error Handling

The class error_handler defines error codes and provides error messages custom for each error code.

<table>
    <thead>
        <tr>
            <th>Error Category</th>
            <th>Code</th>
            <th>Name</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan=3>General Errors</td>
            <td>-11</td>
            <td>ERROR_UNKNOWN</td>
            <td>Unknown Error</td>
        </tr>
        <tr>
            <td>-12</td>
            <td>ERROR_MEMORY_FAIL</td>
            <td>Memory allocation failed</td>
        </tr>
        <tr>
            <td>-13</td>
            <td>ERROR_NO_SENSOR</td>
            <td>Specified sensor not found</td>
        </tr>
        <tr>
            <td rowspan=1>CPU Temperature</td>
            <td>-21</td>
            <td>ERROR_READ_FAIL</td>
            <td>Failed to read temperature data</td>
        </tr>
        <tr>
            <td rowspan=1>CMD Control</td>
            <td>-31</td>
            <td>ERROR_INVALID_ARGS</td>
            <td>Invalid arguments</td>
        </tr>
        <tr>
            <td rowspan=1>LED Control</td>
            <td>-41</td>
            <td>ERROR_LED_WRITE</td>
            <td>Failed to write led data</td>
        </tr>
    </tbody>
</table>


## Convert Timestamps

The class timestamp_convert converts a timestamp (&micro;s) into the format hh:mm:ss.