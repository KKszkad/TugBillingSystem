# TugBillingSystem

## PREFACE

> Need QT 6.X to run the source code. My QT Version is 6.6.0.



This project focuses on the client side of the entire system. The backend has already been deployed on a remote server with the **IP address** 106.54.213.38, which also serves as the default server IP address for the client. Alternatively, you can download the backend of the system for local test.

[Download Server Source Code](https://github.com/KKszkad/Download/raw/main/Server.7z)

And if you want to test the program directly, please just download the release version of the client.

[Download Client Release Version](https://github.com/KKszkad/Download/raw/main/Client.7z)



## Features

The system consists of three user roles:

**Ship Agency:**

- Responsible for submitting applications. Select the works that require Tugboat Company to do.
- Accesses a table where all applications are listed.
- Has the authority to approve or deny quotations (expenses edited by Billing Clerks and approved by Audit Clerks). If approved, the application concludes, and they can request invoices from the Audit Clerk. If denied, the Billing Clerk must reedit the expense, initiating a new round of review.

**Billing Clerk:**

- Accesses a table where all applications are listed.
- Edits the expenses of each works included in applications as needed.

**Audit Clerk:**

- Accesses a table with a list of all applications.
- Approves or denies applications edited by Billing Clerks. If denied, Billing Clerks must revise the expense.
- Can print invoices for applications approved by the Ship Agency.

The diagram below illustrates the ideal process flow of the system.

![Process Flow](https://raw.githubusercontent.com/KKszkad/Pictures/main/TugBillingSystem/1.png)

**NOTE:** Each application is sequentially handled by one user role at a time based on its status. Both the Audit Clerk and Ship Agency have the ability to deny expenses edited by Billing Clerks, triggering a reevaluation of the checking process.


The client's appearance may not be aesthetically pleasing, but my focus is on refining my C++ and general programming skills, prioritizing the functionality of the system.



## USAGE

The original project was developed in Chinese, so the system's data is also in Chinese. Here is some information to facilitate your testing.



**Accounts Available ：**

| CA            | Abbreviation | User Name | Password | User Type |
| ------------- | ------------ | --------- | -------- | --------- |
| Ship Agency   | SA           | 张三      | admin    |           |
| Billing Clerk | BC           | 李四      | admin    |           |
| Audit Clerk   | AC           | Clerk     | admin    |           |





After you login as 张三, the Ship Agency, you can click **Application** to get to this page.



![First Step](https://raw.githubusercontent.com/KKszkad/Pictures/main/TugBillingSystem/2.png)

When you start a new application, you will need to enter the Chinese name of the ship. You can try to copy the ship name list below:

* 蓉达天津
* 卡娜
* 盖尔
* 昌明2

In completing the application, there should be no obstacles, as certain information is not mandatory for submission.



To interact with the table, simply double-click on the item inside it. Please be aware that only the rows marked with colored **status** are accessible based on the current process and your account privileges.



Even if you are not familiar with Chinese (the language of our data), you can identify your application by checking the submission time.