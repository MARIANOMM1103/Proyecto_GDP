#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <limits>   // para numeric_limits
#include <cstdio>   
#include <cstring> 

using namespace std;

// =================================================
//       Gestionamos la conexión
// =================================================
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

    // Ajusta estos parámetros según tu entorno:
    conn = mysql_real_connect(
        conn, 
        "localhost",    // Host
        "usuario",      // Usuario de MySQL
        "password",     // Contraseña de MySQL
        "basedatos",    // Nombre de la base de datos
        3306,           // Puerto
        NULL,           // Socket
        0
    );

    if (!conn) {
        cerr << "Error al conectar: " << mysql_error(conn) << endl;
        return nullptr;
    }
    return conn;
}

// =================================================
//      Clase usuario para poder gestionar
// =================================================
class Usuario {
public:
    static bool iniciarSesion(MYSQL* conn, const string& usuario, const string& contrasena);
};

bool Usuario::iniciarSesion(MYSQL* conn, const string& usuario, const string& contrasena) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return false;
    }

    // Consulta parametrizada para evitar inyección SQL
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

    // Enlazar parámetros (usuario, contrasena)
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    // Parámetro 1 (usuario)
    bind[0].buffer_type   = MYSQL_TYPE_STRING;
    bind[0].buffer        = (char*)usuario.c_str();
    bind[0].buffer_length = usuario.length();

    // Parámetro 2 (contrasena)
    bind[1].buffer_type   = MYSQL_TYPE_STRING;
    bind[1].buffer        = (char*)contrasena.c_str();
    bind[1].buffer_length = contrasena.length();

    // Vinculamos e ejecutamos
    mysql_stmt_bind_param(stmt, bind);
    if (mysql_stmt_execute(stmt) != 0) {
        cerr << "Error al ejecutar statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // Enlazamos el resultado (el COUNT(*))
    MYSQL_BIND result_bind;
    memset(&result_bind, 0, sizeof(result_bind));

    int count = 0;
    result_bind.buffer_type = MYSQL_TYPE_LONG;
    result_bind.buffer = &count;

    if (mysql_stmt_bind_result(stmt, &result_bind) != 0) {
        cerr << "Error al enlazar resultado: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_fetch(stmt) == MYSQL_NO_DATA) {
        cerr << "No se pudieron obtener datos del statement." << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return (count > 0);
}


class Empleado {
public:
    int id;
    string curp;
    string nombreCompleto;
    string puesto;
    string direccion;
    string empresaAsignada;
    string tipoContrato;
    string numeroCredencial;
    string estado;
    string fechaIngreso;

    // Registrar un nuevo empleado
    void registrar(MYSQL* conn);

    // Editar un empleado existente
    static void editar(MYSQL* conn);

    // Eliminar un empleado por ID
    static void eliminar(MYSQL* conn);

    // Buscar un empleado por ID
    static void buscar(MYSQL* conn);
};

void Empleado::registrar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Registrar Nuevo Empleado ---\n";
    cout << "ID (número): ";
    cin >> id;
    // Limpiar buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    cout << "CURP (18 caracteres): ";
    getline(cin, curp);
    if (curp.size() != 18) {
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

    cout << "Tipo de Contrato (ej. '40h', '48h'): ";
    getline(cin, tipoContrato);

    cout << "Numero de Credencial (opcional): ";
    getline(cin, numeroCredencial);

    cout << "Estado (Prueba/Completo): ";
    getline(cin, estado);

    cout << "Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, fechaIngreso);

    // Construimos la consulta de inserción
    string query =
        "INSERT INTO empleados (id, curp, nombreCompleto, puesto, direccion, "
        "empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) "
        "VALUES (" + to_string(id) + ", '" + curp + "', '" + nombreCompleto + "', '" 
        + puesto + "', '" + direccion + "', '" + empresaAsignada + "', '"
        + tipoContrato + "', '" + numeroCredencial + "', '" + estado + "', '"
        + fechaIngreso + "')";

    // Se ejecuta la consulta
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado registrado correctamente." << endl;
    } else {
        cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::editar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Editar Empleado ---\n";
    cout << "Ingrese el ID del empleado a editar: ";
    int idEditar;
    cin >> idEditar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Verificamos si existe el empleado
    {
        string checkQuery = "SELECT id FROM empleados WHERE id = " + to_string(idEditar);
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
            return;
        }
        // Si no hay filas, no existe
        if (mysql_num_rows(res) == 0) {
            cout << "No se encontró un empleado con ese ID." << endl;
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
    }

    // Pedimos los datos actualizados al usuario
    string nuevoNombre, nuevoPuesto, nuevaDireccion, nuevaEmpresa, nuevoTipo;
    string nuevaCredencial, nuevoEstado, nuevaFecha;
    cout << "Ingrese el nuevo Nombre Completo: ";
    getline(cin, nuevoNombre);
    cout << "Ingrese el nuevo Puesto: ";
    getline(cin, nuevoPuesto);
    cout << "Ingrese la nueva Direccion: ";
    getline(cin, nuevaDireccion);
    cout << "Ingrese la nueva Empresa Asignada: ";
    getline(cin, nuevaEmpresa);
    cout << "Ingrese el nuevo Tipo de Contrato: ";
    getline(cin, nuevoTipo);
    cout << "Ingrese la nueva Credencial: ";
    getline(cin, nuevaCredencial);
    cout << "Ingrese el nuevo Estado (Prueba/Completo): ";
    getline(cin, nuevoEstado);
    cout << "Ingrese la nueva Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, nuevaFecha);

    // Creamos la consulta UPDATE
    string updateQuery =
        "UPDATE empleados SET "
        "nombreCompleto = '" + nuevoNombre + "', "
        "puesto = '" + nuevoPuesto + "', "
        "direccion = '" + nuevaDireccion + "', "
        "empresaAsignada = '" + nuevaEmpresa + "', "
        "tipoContrato = '" + nuevoTipo + "', "
        "numeroCredencial = '" + nuevaCredencial + "', "
        "estado = '" + nuevoEstado + "', "
        "fechaIngreso = '" + nuevaFecha + "' "
        "WHERE id = " + to_string(idEditar);

    if (mysql_query(conn, updateQuery.c_str()) == 0) {
        cout << "Empleado actualizado correctamente." << endl;
    } else {
        cerr << "Error al actualizar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::eliminar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Eliminar Empleado ---\n";
    cout << "Ingrese el ID del empleado a eliminar: ";
    int idEliminar;
    cin >> idEliminar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Verificamos si el empleado existe
    {
        string checkQuery = "SELECT id FROM empleados WHERE id = " + to_string(idEliminar);
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
            return;
        }
        if (mysql_num_rows(res) == 0) {
            cout << "No se encontró un empleado con ese ID." << endl;
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
    }

    // Creamos y ejecutamos la consulta DELETE
    string deleteQuery = "DELETE FROM empleados WHERE id = " + to_string(idEliminar);
    if (mysql_query(conn, deleteQuery.c_str()) == 0) {
        cout << "Empleado eliminado correctamente." << endl;
    } else {
        cerr << "Error al eliminar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::buscar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Buscar Empleado ---\n";
    cout << "Ingrese el ID del empleado a buscar: ";
    int idBuscar;
    cin >> idBuscar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Creamos la consulta SELECT
    string selectQuery =
        "SELECT id, curp, nombreCompleto, puesto, direccion, empresaAsignada, "
        "tipoContrato, numeroCredencial, estado, fechaIngreso "
        "FROM empleados WHERE id = " + to_string(idBuscar);

    if (mysql_query(conn, selectQuery.c_str()) != 0) {
        cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
        return;
    }

    // Procesamos el resultado
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
        return;
    }

    // Si no hay filas, el empleado no existe
    if (mysql_num_rows(res) == 0) {
        cout << "No se encontró un empleado con ese ID." << endl;
        mysql_free_result(res);
        return;
    }

    // Tomamos la primera fila (asumiendo ID único)
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        cout << "\n--- Datos del Empleado ---\n";
        cout << "ID: " << (row[0] ? row[0] : "") << endl;
        cout << "CURP: " << (row[1] ? row[1] : "") << endl;
        cout << "Nombre Completo: " << (row[2] ? row[2] : "") << endl;
        cout << "Puesto: " << (row[3] ? row[3] : "") << endl;
        cout << "Direccion: " << (row[4] ? row[4] : "") << endl;
        cout << "Empresa Asignada: " << (row[5] ? row[5] : "") << endl;
        cout << "Tipo de Contrato: " << (row[6] ? row[6] : "") << endl;
        cout << "Numero de Credencial: " << (row[7] ? row[7] : "") << endl;
        cout << "Estado: " << (row[8] ? row[8] : "") << endl;
        cout << "Fecha de Ingreso: " << (row[9] ? row[9] : "") << endl;
    }

    mysql_free_result(res);
}
//Función para mostrar el menú//
void mostrarMenu(MYSQL* conn) {
    int opcion;
    do {
        cout << "\n--- Menú Principal ---\n";
        cout << "1. Registrar Empleado\n";
        cout << "2. Editar Empleado\n";
        cout << "3. Eliminar Empleado\n";
        cout << "4. Buscar Empleado\n";
        cout << "5. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opcion) {
        case 1: {
            // Registrar
            Empleado emp;
            emp.registrar(conn);
            break;
        }
        case 2:
            // Editar
            Empleado::editar(conn);
            break;
        case 3:
            // Eliminar
            Empleado::eliminar(conn);
            break;
        case 4:
            // Buscar
            Empleado::buscar(conn);
            break;
        case 5:
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opción no válida. Intente nuevamente." << endl;
        }
    } while (opcion != 5);
}


int main() {
    // 1) Conectar a la base de datos
    MYSQL* conn = ConexionDB::conectar();
    if (!conn) {
        // Si la conexión falla, terminamos
        return 1;
    }

    // 2) Pedir credenciales de usuario
    string usuario, contrasena;
    cout << "Usuario: ";
    cin >> usuario;
    cout << "Contraseña: ";
    cin >> contrasena;

    // 3) Intentar iniciar sesión
    if (!Usuario::iniciarSesion(conn, usuario, contrasena)) {
        cerr << "Error: Credenciales incorrectas." << endl;
        mysql_close(conn);
        return 1;
    }

    cout << "Inicio de sesión exitoso." << endl;

    // 4) Mostrar el menú principal
    mostrarMenu(conn);

    // 5) Cerrar la conexión
    mysql_close(conn);
    return 0;
}
