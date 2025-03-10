#include <iostream>
#include <string>
#include <limits>
#include <mysql/mysql.h>
using namespace std;

// Estructura para empleados
struct Empleado {
    string nombreCompleto;
    int id;
    string puesto;
    string curp;
    string direccion;       // Se corrigió "direcccion"
    string empresaAsignada;
    string tipoContrato;    
    string numeroCredencial;
    string estado;          
    string fechaIngreso;
};

// Función para conectar a la base de datos
MYSQL* conectar() {
    MYSQL* conn = mysql_init(nullptr);

    // Sustituye los argumentos por los correctos para tu servidor y tu base de datos
    // Ejemplo: (conn, "localhost", "usuario", "contraseña", "nombre_db", 3306, NULL, 0);
    conn = mysql_real_connect(conn, "Aqui van los datos que nos tiene que pasar kevin", 3306, nullptr, nullptr, 0, nullptr, 0);

    if (conn) {
        cout << "Conexion exitosa.\n";
    } else {
        cerr << "Error al conectar: " << mysql_error(conn) << endl;
    }
    return conn;
}

// Función para registrar un empleado en la base de datos
void registrarEmpleado(MYSQL* conn) {
    if (!conn) {
        cerr << "No hay conexion con la base de datos.\n";
        return;
    }

    Empleado e;
    cout << "\n--- Registrar Nuevo Empleado ---\n";

    cout << "ID (numero): ";
    cin >> e.id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer

    cout << "Nombre Completo: ";
    getline(cin, e.nombreCompleto);

    cout << "Puesto: ";
    getline(cin, e.puesto);

    cout << "CURP: ";
    getline(cin, e.curp);

    cout << "Direccion: ";
    getline(cin, e.direccion);

    cout << "Empresa Asignada: ";
    getline(cin, e.empresaAsignada);

    cout << "Tipo de Contrato (40/48 horas): ";
    getline(cin, e.tipoContrato);

    cout << "Numero de Credencial (opcional): ";
    getline(cin, e.numeroCredencial);

    cout << "Estado (Prueba/Completo): ";
    getline(cin, e.estado);

    cout << "Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, e.fechaIngreso);

    // Crear la consulta SQL para insertar
    string query = "INSERT INTO empleados "
                   "(id, nombreCompleto, puesto, curp, direccion, empresaAsignada, "
                   " tipoContrato, numeroCredencial, estado, fechaIngreso) VALUES (" +
                   to_string(e.id) + ", '" + e.nombreCompleto + "', '" + e.puesto + 
                   "', '" + e.curp + "', '" + e.direccion + "', '" + e.empresaAsignada +
                   "', '" + e.tipoContrato + "', '" + e.numeroCredencial + "', '" +
                   e.estado + "', '" + e.fechaIngreso + "')";

    // Ejecutar la consulta
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado registrado correctamente.\n";
    } else {
        cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
    }
}

// MENÚ PRINCIPAL - BÁSICO
int main() {
    MYSQL* conn = conectar();  // Establecemos conexión con la base de datos
    if (!conn) {
        // Si no hay conexión, terminamos la ejecución
        cerr << "No se pudo establecer conexion con la base de datos.\n";
        return 1;
    }

    bool salir = false;
    while (!salir) {
        cout << "\n--- Menú Principal ---\n";
        cout << "1. Gestión de Empleados\n";
        cout << "2. Reportes\n";
        cout << "3. Salir\n";
        cout << "Seleccione una opción: ";

        int opcion;
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer

        switch (opcion) {
            case 1:
                // Aquí podemos registrar empleados (u otras funciones de gestión)
                registrarEmpleado(conn);
                break;
            case 2:
                cout << "\n--- Sección de Reportes (Por implementar) ---\n";
                // Aquí irían las funciones para mostrar reportes, estadísticas, etc.
                break;
            case 3:
                cout << "\nSaliendo del programa...\n";
                salir = true;
                break;
            default:
                cout << "\nOpción inválida. Intente de nuevo.\n";
                break;
        }
    }

    mysql_close(conn); // Cerramos la conexión al terminar
    return 0;
}
