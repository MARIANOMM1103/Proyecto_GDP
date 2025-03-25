#include <iostream>
#include <string>
#include <limits>       // Para usar numeric_limits
#include <mysql/mysql.h>

using namespace std;

//---------------------------------------------------------
// Clase para gestionar la conexión a la base de datos
//---------------------------------------------------------
class ConexionDB {
public:
    static MYSQL* conectar();
};

MYSQL* ConexionDB::conectar() {
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) {
        cerr << "Error al inicializar MySQL." << endl;
        return nullptr;
    }

    // Ajusta estos valores con tus propios datos de conexión:
    // host, usuario, contraseña, base_de_datos, puerto...
    conn = mysql_real_connect(
        conn,
        "localhost",   // Host o IP
        "usuario",     // Usuario de la BD
        "password",    // Contraseña de la BD
        "basedatos",   // Nombre de la BD
        3306,          // Puerto (opcional si es el default 3306)
        NULL,
        0
    );

    if (!conn) {
        cerr << "Error al conectar: " << mysql_error(conn) << endl;
    }
    return conn;
}

//---------------------------------------------------------
// Clase para gestionar el inicio de sesión del usuario
//---------------------------------------------------------
class Usuario {
public:
    static bool iniciarSesion(MYSQL* conn, const string& usuario, const string& contrasena);
};

bool Usuario::iniciarSesion(MYSQL* conn, const string& usuario, const string& contrasena) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return false;
    }

    // Uso de consultas preparadas para evitar inyección SQL
    const char* query = "SELECT COUNT(*) FROM usuarios WHERE usuario = ? AND contrasena = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Error al inicializar statement: " << mysql_error(conn) << endl;
        return false;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Error al preparar statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // Vinculamos los parámetros (usuario y contrasena)
    MYSQL_BIND bind[2] = {};
    bind[0].buffer_type   = MYSQL_TYPE_STRING;
    bind[0].buffer        = (char*)usuario.c_str();
    bind[0].buffer_length = usuario.length();

    bind[1].buffer_type   = MYSQL_TYPE_STRING;
    bind[1].buffer        = (char*)contrasena.c_str();
    bind[1].buffer_length = contrasena.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        cerr << "Error al bindear parámetros: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        cerr << "Error al ejecutar statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // Preparamos el bind para el resultado (la cantidad de filas)
    MYSQL_BIND result_bind = {};
    int count = 0;
    result_bind.buffer_type   = MYSQL_TYPE_LONG;
    result_bind.buffer        = &count;

    if (mysql_stmt_bind_result(stmt, &result_bind) != 0) {
        cerr << "Error al bindear resultado: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // Obtenemos el resultado
    mysql_stmt_fetch(stmt);
    mysql_stmt_close(stmt);

    // Si count > 0, el usuario/contraseña coinciden
    return (count > 0);
}

//---------------------------------------------------------
// Clase "empleado" para gestionar los datos de un empleado
//---------------------------------------------------------
class Empleado {
public:
    int id;
    string nombreCompleto;
    string puesto;
    string curp;
    string direccion;
    string empresaAsignada;
    string tipoContrato;
    string numeroCredencial;
    string estado;
    string fechaIngreso;

    void registrar(MYSQL* conn);
};

void Empleado::registrar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Registrar Nuevo Empleado ---\n";

    cout << "ID (número): ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer

    cout << "CURP (18 caracteres): ";
    getline(cin, curp);
    if (curp.length() != 18) {
        cerr << "Error: CURP inválido. Debe tener 18 caracteres." << endl;
        return;
    }

    cout << "Nombre Completo: ";
    getline(cin, nombreCompleto);

    cout << "Puesto: ";
    getline(cin, puesto);

    cout << "Direccion: ";
    getline(cin, direccion);

    cout << "Empresa Asignada: ";
    getline(cin, empresaAsignada);

    cout << "Tipo de Contrato (ej. '40h' o '48h'): ";
    getline(cin, tipoContrato);

    cout << "Numero de Credencial (opcional): ";
    getline(cin, numeroCredencial);

    cout << "Estado (Prueba/Completo): ";
    getline(cin, estado);

    cout << "Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, fechaIngreso);

    // ------------------------------------------------
    // Inserción en la base de datos con mysql_query
    // ------------------------------------------------
    // 
    string query =
        "INSERT INTO empleados (id, nombreCompleto, puesto, curp, direccion, empresaAsignada, "
        "tipoContrato, numeroCredencial, estado, fechaIngreso) VALUES (" 
        + to_string(id) + ", '" + nombreCompleto + "', '" + puesto + "', '" + curp + "', '" 
        + direccion + "', '" + empresaAsignada + "', '" + tipoContrato + "', '"
        + numeroCredencial + "', '" + estado + "', '" + fechaIngreso + "')";

    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado registrado correctamente." << endl;
    } else {
        cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
    }
}

//---------------------------------------------------------
// Función que muestra el menú principal de la aplicación
//---------------------------------------------------------
void mostrarMenu(MYSQL* conn) {
    int opcion;
    do {
        cout << "\n--- Menú Principal ---\n";
        cout << "1. Gestionar Empleado" << endl;
        cout << "2. Calcular Nómina" << endl;
        cout << "3. Asignar Bonificaciones" << endl;
        cout << "4. Generar Reportes" << endl;
        cout << "5. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer

        switch (opcion) {
            case 1: {
                Empleado emp;
                emp.registrar(conn);
                break;
            }
            case 2:
                cout << "Función para calcular nómina aún no implementada." << endl;
                break;
            case 3:
                cout << "Función para asignar bonificaciones aún no implementada." << endl;
                break;
            case 4:
                cout << "Función para generar reportes aún no implementada." << endl;
                break;
            case 5:
                cout << "Saliendo..." << endl;
                break;
            default:
                cout << "Opción no válida. Intente nuevamente." << endl;
        }
    } while (opcion != 5);
}

//---------------------------------------------------------
// Función principal (main)
//---------------------------------------------------------
int main() {
    // 1) Conexión a la base de datos
    MYSQL* conn = ConexionDB::conectar();
    if (!conn) {
        // Si no se pudo conectar, salimos.
        return 1;
    }

    // 2) Inicio de sesión
    string usuario, contrasena;
    cout << "Usuario: ";
    cin >> usuario;
    cout << "Contraseña: ";
    cin >> contrasena;

    if (!Usuario::iniciarSesion(conn, usuario, contrasena)) {
        cerr << "Error: Credenciales incorrectas." << endl;
        mysql_close(conn);
        return 1;
    }

    cout << "Inicio de sesión exitoso." << endl;

    // 3) Mostramos el menú principal
    mostrarMenu(conn);

    // 4) Cerramos la conexión antes de terminar
    mysql_close(conn);
    return 0;
}
