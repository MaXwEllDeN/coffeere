import os
import sqlite3

if not "DATABASE_FILE" in os.environ:
    print("DATABASE_FILE not defined. Exiting...")
    exit()

DATABASE_FILE = os.environ["DATABASE_FILE"]

class Database:
    def __connect(self):
        return sqlite3.connect(DATABASE_FILE)

    def get_all_subscriptions(self):
        conn = self.__connect()

        cursor = conn.cursor()
        cursor.execute("SELECT * FROM `subscriptions`;")
        data = cursor.fetchall()
        conn.close()

        return data

    def add_subscription(self, sub_str):
        conn = self.__connect()
        
        cursor = conn.cursor()
        try:
            cursor.execute("INSERT INTO `subscriptions` (`sub_str`) VALUES ('%s');" % (sub_str))
            conn.commit()
        except:
            conn.rollback()

        conn.close()
        return True

    def remove_subscription(self, id):
        conn = self.__connect()
        
        cursor = conn.cursor()
        try:
            cursor.execute("DELETE FROM `subscriptions` WHERE `id` = '%s';" % id)
            #AND `notification` = '%s';" % (username, notification))
            conn.commit()
        except:
            conn.rollback()

        conn.close()
        return True

    def is_user_registered(self, username):
        user = self.get_user(username)
        return user is not None


    def create_db(self):
        if os.path.isfile(DATABASE_FILE):
            return False

        conn = self.__connect()

        try:
            conn.execute('''CREATE TABLE users
                        (id INT PRIMARY KEY     NOT NULL,
                        username        CHAR(50),
                        name           TEXT    NOT NULL);''')

            conn.execute('''CREATE TABLE subscriptions
                        (id SERIAL PRIMARY KEY,
                        sub_str        TEXT    NOT NULL);''')
            conn.commit()
            print("Table created successfully")
        except:
            conn.rollback()

        conn.close()
        return True
