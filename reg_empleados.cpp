#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <conio.h>
#include <mysql/jdbc.h>

using namespace std;

// ------------------------------
// Conexión a la base de datos
// ------------------------------
class ConexionDB {
public:
    static sql::Connection* conectar() {
        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            sql::Connection* conn = driver->connect("tcp://127.0.0.1:3306", "root", "proot");
            conn->setSchema("proyecto_cplusplus");
            return conn;
        }
        catch (sql::SQLException& e) {
            cerr << "Error al conectar: " << e.what() << endl;
            return nullptr;
        }
    }
};

// ------------------------------
// Entrada oculta de contraseña
// ------------------------------
string obtenerContrasena() {
    string contrasena;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !contrasena.empty()) {
            cout << "\b \b";
            contrasena.pop_back();
        } else if (ch != '\b') {
            cout << '*';
            contrasena += ch;
        }
    }
    cout << endl;
    return contrasena;
}

// ------------------------------
// Módulo de Usuario
// ------------------------------
class Usuario {
public:
    static bool iniciarSesion(sql::Connection* conn, const string& usuario, const string& contrasena) {
        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("SELECT COUNT(*) FROM usuarios WHERE usuario = ? AND contrasena = ?")
            );
            stmt->setString(1, usuario);
            stmt->setString(2, contrasena);
            unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            return res->next() && res->getInt(1) > 0;
        } catch (sql::SQLException& e) {
            cerr << "Error SQL (login): " << e.what() << endl;
            return false;
        }
    }
};

// ------------------------------
// Módulo de Empleado
// ------------------------------
class Empleado {
public:
    int id;
    string curp, nombreCompleto, puesto, direccion, empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso;

    void registrar(sql::Connection* conn) {
        cout << "\n--- Registrar Empleado ---\n";
        cout << "ID: "; cin >> id; cin.ignore();
        cout << "CURP: "; getline(cin, curp);
        cout << "Nombre: "; getline(cin, nombreCompleto);
        cout << "Puesto: "; getline(cin, puesto);
        cout << "Direccion: "; getline(cin, direccion);
        cout << "Empresa: "; getline(cin, empresaAsignada);
        cout << "Tipo de contrato: "; getline(cin, tipoContrato);
        cout << "No. Credencial: "; getline(cin, numeroCredencial);
        cout << "Estado: "; getline(cin, estado);
        cout << "Fecha Ingreso (YYYY-MM-DD): "; getline(cin, fechaIngreso);

        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement(
                    "INSERT INTO empleados (id, curp, nombreCompleto, puesto, direccion, empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                )
            );
            stmt->setInt(1, id);
            stmt->setString(2, curp);
            stmt->setString(3, nombreCompleto);
            stmt->setString(4, puesto);
            stmt->setString(5, direccion);
            stmt->setString(6, empresaAsignada);
            stmt->setString(7, tipoContrato);
            stmt->setString(8, numeroCredencial);
            stmt->setString(9, estado);
            stmt->setString(10, fechaIngreso);
            stmt->execute();
            cout << "Empleado registrado con éxito.\n";
        } catch (sql::SQLException& e) {
            cerr << "Error al registrar: " << e.what() << endl;
        }
    }

    static void editar(sql::Connection* conn) {
        int id;
        string nuevoPuesto;
        cout << "ID del empleado a editar: ";
        cin >> id; cin.ignore();
        cout << "Nuevo puesto: ";
        getline(cin, nuevoPuesto);
        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("UPDATE empleados SET puesto = ? WHERE id = ?")
            );
            stmt->setString(1, nuevoPuesto);
            stmt->setInt(2, id);
            int result = stmt->executeUpdate();
            cout << (result > 0 ? "Empleado actualizado.\n" : "Empleado no encontrado.\n");
        } catch (sql::SQLException& e) {
            cerr << "Error al editar: " << e.what() << endl;
        }
    }

    static void eliminar(sql::Connection* conn) {
        int id;
        cout << "ID del empleado a eliminar: ";
        cin >> id; cin.ignore();
        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("DELETE FROM empleados WHERE id = ?")
            );
            stmt->setInt(1, id);
            int res = stmt->executeUpdate();
            cout << (res > 0 ? "Empleado eliminado.\n" : "Empleado no encontrado.\n");
        } catch (sql::SQLException& e) {
            cerr << "Error al eliminar: " << e.what() << endl;
        }
    }

    static void buscar(sql::Connection* conn) {
        int id;
        cout << "ID del empleado: ";
        cin >> id; cin.ignore();
        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("SELECT * FROM empleados WHERE id = ?")
            );
            stmt->setInt(1, id);
            unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            if (res->next()) {
                cout << "Nombre: " << res->getString("nombreCompleto") << "\n";
                cout << "Puesto: " << res->getString("puesto") << "\n";
                cout << "Empresa: " << res->getString("empresaAsignada") << "\n";
            } else {
                cout << "Empleado no encontrado.\n";
            }
        } catch (sql::SQLException& e) {
            cerr << "Error al buscar: " << e.what() << endl;
        }
    }
};

// ------------------------------
// Módulo de Nómina
// ------------------------------
class Nomina {
public:
    static void registrar(sql::Connection* conn) {
        int idEmpleado;
        double sueldoBase, bono;
        string fechaPago;

        cout << "\n--- Registrar Nómina ---\n";
        cout << "ID del empleado: ";
        cin >> idEmpleado; cin.ignore();
        cout << "Sueldo base: ";
        cin >> sueldoBase; cin.ignore();
        cout << "Bono: ";
        cin >> bono; cin.ignore();
        cout << "Fecha de pago (YYYY-MM-DD): ";
        getline(cin, fechaPago);

        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("INSERT INTO nominas (idEmpleado, sueldoBase, bono, fechaPago) VALUES (?, ?, ?, ?)")
            );
            stmt->setInt(1, idEmpleado);
            stmt->setDouble(2, sueldoBase);
            stmt->setDouble(3, bono);
            stmt->setString(4, fechaPago);
            stmt->execute();

            double total = sueldoBase + bono;
            cout << "\n--- Recibo de Nómina ---\n";
            cout << "Empleado ID: " << idEmpleado << "\n";
            cout << "Sueldo base: $" << sueldoBase << "\n";
            cout << "Bono: $" << bono << "\n";
            cout << "Total pagado: $" << total << "\n";
            cout << "Fecha: " << fechaPago << "\n";
        } catch (sql::SQLException& e) {
            cerr << "Error al registrar nómina: " << e.what() << endl;
        }
    }

    static void darBono(sql::Connection* conn) {
        int idNomina;
        double bonoExtra;

        cout << "\n--- Dar Bono Extra ---\n";
        cout << "ID de la nómina: ";
        cin >> idNomina; cin.ignore();
        cout << "Monto del bono extra: ";
        cin >> bonoExtra; cin.ignore();

        try {
            unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement("UPDATE nominas SET bono = bono + ? WHERE id = ?")
            );
            stmt->setDouble(1, bonoExtra);
            stmt->setInt(2, idNomina);
            int filas = stmt->executeUpdate();
            cout << (filas > 0 ? "Bono añadido exitosamente.\n" : "Nómina no encontrada.\n");
        } catch (sql::SQLException& e) {
            cerr << "Error al dar bono: " << e.what() << endl;
        }
    }
};

// ------------------------------
// Menú Principal
// ------------------------------
void mostrarMenu(sql::Connection* conn) {
    int opcion;
    do {
        cout << "\n--- Menú ---\n";
        cout << "1. Registrar empleado\n";
        cout << "2. Editar empleado\n";
        cout << "3. Eliminar empleado\n";
        cout << "4. Buscar empleado\n";
        cout << "5. Registrar nómina\n";
        cout << "6. Dar bono extra\n";
        cout << "7. Cerrar sesión\n";
        cout << "8. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> opcion; cin.ignore();

        switch (opcion) {
            case 1: { Empleado e; e.registrar(conn); break; }
            case 2: Empleado::editar(conn); break;
            case 3: Empleado::eliminar(conn); break;
            case 4: Empleado::buscar(conn); break;
            case 5: Nomina::registrar(conn); break;
            case 6: Nomina::darBono(conn); break;
            case 7: cout << "Cerrando sesión...\n"; return;
            case 8: cout << "Saliendo...\n"; exit(0);
            default: cout << "Opción inválida.\n";
        }
    } while (true);
}

// ------------------------------
// Función principal
// ------------------------------
int main() {
    sql::Connection* conn = ConexionDB::conectar();
    if (!conn) return 1;

    int intentos = 3;
    string usuario, contrasena;

    while (intentos--) {
        cout << "\nUsuario: ";
        cin >> usuario;
        cout << "Contraseña: ";
        contrasena = obtenerContrasena();

        if (Usuario::iniciarSesion(conn, usuario, contrasena)) {
            cout << "\nSesión iniciada.\n";
            mostrarMenu(conn);
            break;
        } else {
            cout << "Credenciales incorrectas.\n";
            if (intentos > 0) {
                cout << "Intentos restantes: " << intentos << ". Esperando 30 segundos...\n";
                this_thread::sleep_for(chrono::seconds(30));
            } else {
                cout << "Demasiados intentos fallidos. Cerrando.\n";
                delete conn;
                return 1;
            }
        }
    }

    delete conn;
    return 0;
}

  
