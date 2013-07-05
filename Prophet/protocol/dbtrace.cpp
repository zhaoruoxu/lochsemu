#include "stdafx.h"
#include "dbtrace.h"
#include "message.h"
#include "runtrace.h"
#include "static/disassembler.h"
#include "sqlite/SQLiteC++.h"

DBTrace::DBTrace()
{
    m_instid = 0;
}

DBTrace::~DBTrace()
{
    SAFE_DELETE(m_stmtInsertMessage);
    SAFE_DELETE(m_stmtInsertMessageData);
    SAFE_DELETE(m_stmtInsertInst);
    SAFE_DELETE(m_stmtInsertDisasm);
    SAFE_DELETE(m_db);
}

void DBTrace::Initialize( const char * dbpath )
{
    LxInfo("Creating database %s\n", dbpath);
    m_db = new SQLite::Database(dbpath, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

    InitTables();
    InitStatements();
}

void DBTrace::Serialize( Json::Value &root ) const 
{

}

void DBTrace::Deserialize( Json::Value &root )
{

}

void DBTrace::InitTables()
{
    LxInfo("DBTrace: Initializing tables\n");
    SQLite::Transaction t(*m_db);
    m_db->exec("DROP TABLE IF EXISTS messages");
    m_db->exec("DROP TABLE IF EXISTS messages_data");
    m_db->exec("DROP TABLE IF EXISTS insts");
    m_db->exec("DROP TABLE IF EXISTS disasm");

    m_db->exec("CREATE TABLE messages (id INTEGER PRIMARY KEY, "
        "trace_count INTEGER, length INTEGER)");
    m_db->exec("CREATE TABLE messages_data (id INTEGER, offset INTEGER, "
        "data INTEGER, FOREIGN KEY(id) REFERENCES messages(id))");
    m_db->exec("CREATE TABLE insts (id BIGINT, msg_id INTEGER,"
        "eax INTEGER, ecx INTEGER, edx INTEGER, ebx INTEGER,"
        "esp INTEGER, ebp INTEGER, esi INTEGER, edi INTEGER,"
        "eip INTEGER, eflags INTEGER,"
        "mr_addr INTEGER, mr_len INTEGER, mr_val INTEGER,"
        "mw_addr INTEGER, mw_len INTEGER, mw_val INTEGER,"
        "tid INTEGER, ext_tid INTEGER, jump_taken BOOL, exec_flag INTEGER"
        ")");
    m_db->exec("CREATE TABLE disasm (eip INTEGER, instr VARCHAR(64),"
        "category INTEGER, opcode INTEGER, mnemonic VARCHAR(16),"
        "branch_type INTEGER, addr_val INTEGER, immediate INTEGER,"
        "a1_mnemonic VARCHAR(32), a1_type INTEGER, a1_size INTEGER, a1_pos INTEGER,"
        "a1_basereg INTEGER, a1_indexreg INTEGER, a1_scale INTEGER, a1_disp INTEGER,"
        "a2_mnemonic VARCHAR(32), a2_type INTEGER, a2_size INTEGER, a2_pos INTEGER,"
        "a2_basereg INTEGER, a2_indexreg INTEGER, a2_scale INTEGER, a2_disp INTEGER,"
        "a3_mnemonic VARCHAR(32), a3_type INTEGER, a3_size INTEGER, a3_pos INTEGER,"
        "a3_basereg INTEGER, a3_indexreg INTEGER, a3_scale INTEGER, a3_disp INTEGER,"
        "p_oper BOOL, p_addr BOOL, p_repne BOOL, p_rep BOOL,"
        "aux_opcode INTEGER, aux_modrm INTEGER, aux_sib INTEGER"
        ")");
    t.commit();
}

void DBTrace::InitStatements()
{
    m_stmtInsertMessage = new SQLite::Statement(*m_db, 
        "INSERT INTO messages VALUES ( :id, :trace_count, :length )");
    m_stmtInsertMessageData = new SQLite::Statement(*m_db,
        "INSERT INTO messages_data VALUES ( :id, :offset, :data )");
    m_stmtInsertInst = new SQLite::Statement(*m_db,
        "INSERT INTO insts VALUES ( :id, :msg_id, :eax, :ecx, :edx, :ebx,"
        ":esp, :ebp, :esi, :edi, :eip, :eflags,"
        ":mr_addr, :mr_len, :mr_val, :mw_addr, :mw_len, :mw_val,"
        ":tid, :ext_tid, :jump_taken, :exec_flag"
        " )");
    m_stmtInsertDisasm = new SQLite::Statement(*m_db,
        "INSERT INTO disasm VALUES ( :eip, :instr, :category, :opcode, :mnemonic,"
        ":branch_type, :addr_val, :immediate,"
        ":a1_mnemonic, :a1_type, :a1_size, :a1_pos, :a1_basereg, :a1_indexreg, :a1_scale, :a1_disp,"
        ":a2_mnemonic, :a2_type, :a2_size, :a2_pos, :a2_basereg, :a2_indexreg, :a2_scale, :a2_disp,"
        ":a3_mnemonic, :a3_type, :a3_size, :a3_pos, :a3_basereg, :a3_indexreg, :a3_scale, :a3_disp,"
        ":p_oper, :p_addr, :p_repne, :p_rep, :aux_opcode, :aux_modrm, :aux_sib"
        " )");
}


void DBTrace::TraceMessage( Message *msg )
{
    LxInfo("DBTrace: Tracing message %s id=%d\n", msg->GetName().c_str(), msg->GetID());
    SQLite::Transaction t(*m_db);
    m_stmtInsertMessage->bind(":id", msg->GetID());
    m_stmtInsertMessage->bind(":trace_count", 
        msg->GetTraceEnd() - msg->GetTraceBegin() + 1);
    m_stmtInsertMessage->bind(":length", msg->Size());
    m_stmtInsertMessage->exec();
    m_stmtInsertMessage->reset();

    for (int i = 0; i < msg->Size(); i++) {
        m_stmtInsertMessageData->bind(":id", msg->GetID());
        m_stmtInsertMessageData->bind(":offset", i);
        m_stmtInsertMessageData->bind(":data", msg->Get(i));
        m_stmtInsertMessageData->exec();
        m_stmtInsertMessageData->reset();
    }

    t.commit();

}

void DBTrace::TraceInsts( RunTrace &rt, Message *msg )
{
    LxInfo("DBTrace: Tracing %d instructions in message %s(id=%d)\n",
        rt.Count(), msg->GetName().c_str(), msg->GetID());
    SQLite::Transaction t(*m_db);
    for (int i = 0; i < rt.Count(); i++) {
        const TContext *ctx = rt.Get(i);
        Assert(ctx->Eip == ctx->Inst->Eip);
        TraceInstr(ctx->Inst);
        m_stmtInsertInst->bind(":id", m_instid++);
        m_stmtInsertInst->bind(":msg_id", msg->GetID());
        m_stmtInsertInst->bind(":eax", (int) ctx->EAX);
        m_stmtInsertInst->bind(":ecx", (int) ctx->ECX);
        m_stmtInsertInst->bind(":edx", (int) ctx->EDX);
        m_stmtInsertInst->bind(":ebx", (int) ctx->EBX);
        m_stmtInsertInst->bind(":esp", (int) ctx->ESP);
        m_stmtInsertInst->bind(":ebp", (int) ctx->EBP);
        m_stmtInsertInst->bind(":esi", (int) ctx->ESI);
        m_stmtInsertInst->bind(":edi", (int) ctx->EDI);
        m_stmtInsertInst->bind(":eip", (int) ctx->Eip);
        m_stmtInsertInst->bind(":eflags", (int) ctx->Eflags);
        m_stmtInsertInst->bind(":mr_addr", (int) ctx->Mr.Addr);
        m_stmtInsertInst->bind(":mr_len", (int) ctx->Mr.Len);
        m_stmtInsertInst->bind(":mr_val", (int) ctx->Mr.Val);
        m_stmtInsertInst->bind(":mw_addr", (int) ctx->Mw.Addr);
        m_stmtInsertInst->bind(":mw_len", (int) ctx->Mw.Len);
        m_stmtInsertInst->bind(":mw_val", (int) ctx->Mw.Val);
        m_stmtInsertInst->bind(":tid", (int) ctx->Tid);
        m_stmtInsertInst->bind(":ext_tid", (int) ctx->ExtTid);
        m_stmtInsertInst->bind(":jump_taken", ctx->JumpTaken);
        m_stmtInsertInst->bind(":exec_flag", (int) ctx->ExecFlag);
        m_stmtInsertInst->exec();
        m_stmtInsertInst->reset();
    }
    t.commit();
}

void DBTrace::TraceInstr( InstPtr inst )
{
    if (m_insts.find(inst->Eip) != m_insts.end())
        return;
    m_stmtInsertDisasm->bind(":eip", (int) inst->Eip);
    m_stmtInsertDisasm->bind(":instr", inst->Main.CompleteInstr);
    m_stmtInsertDisasm->bind(":category", inst->Main.Inst.Category);
    m_stmtInsertDisasm->bind(":opcode", inst->Main.Inst.Opcode);
    m_stmtInsertDisasm->bind(":mnemonic", inst->Main.Inst.Mnemonic);
    m_stmtInsertDisasm->bind(":branch_type", inst->Main.Inst.BranchType);
    m_stmtInsertDisasm->bind(":addr_val", (int) ((u32) inst->Main.Inst.AddrValue));
    m_stmtInsertDisasm->bind(":immediate", (int) ((u32) inst->Main.Inst.Immediat));

    m_stmtInsertDisasm->bind(":a1_mnemonic", inst->Main.Argument1.ArgMnemonic);
    m_stmtInsertDisasm->bind(":a1_type", inst->Main.Argument1.ArgType);
    m_stmtInsertDisasm->bind(":a1_size", inst->Main.Argument1.ArgSize);
    m_stmtInsertDisasm->bind(":a1_pos", inst->Main.Argument1.ArgPosition);
    m_stmtInsertDisasm->bind(":a1_basereg", inst->Main.Argument1.Memory.BaseRegister);
    m_stmtInsertDisasm->bind(":a1_indexreg", inst->Main.Argument1.Memory.IndexRegister);
    m_stmtInsertDisasm->bind(":a1_scale", inst->Main.Argument1.Memory.Scale);
    m_stmtInsertDisasm->bind(":a1_disp", inst->Main.Argument1.Memory.Displacement);

    m_stmtInsertDisasm->bind(":a2_mnemonic", inst->Main.Argument2.ArgMnemonic);
    m_stmtInsertDisasm->bind(":a2_type", inst->Main.Argument2.ArgType);
    m_stmtInsertDisasm->bind(":a2_size", inst->Main.Argument2.ArgSize);
    m_stmtInsertDisasm->bind(":a2_pos", inst->Main.Argument2.ArgPosition);
    m_stmtInsertDisasm->bind(":a2_basereg", inst->Main.Argument2.Memory.BaseRegister);
    m_stmtInsertDisasm->bind(":a2_indexreg", inst->Main.Argument2.Memory.IndexRegister);
    m_stmtInsertDisasm->bind(":a2_scale", inst->Main.Argument2.Memory.Scale);
    m_stmtInsertDisasm->bind(":a2_disp", inst->Main.Argument2.Memory.Displacement);

    m_stmtInsertDisasm->bind(":a3_mnemonic", inst->Main.Argument3.ArgMnemonic);
    m_stmtInsertDisasm->bind(":a3_type", inst->Main.Argument3.ArgType);
    m_stmtInsertDisasm->bind(":a3_size", inst->Main.Argument3.ArgSize);
    m_stmtInsertDisasm->bind(":a3_pos", inst->Main.Argument3.ArgPosition);
    m_stmtInsertDisasm->bind(":a3_basereg", inst->Main.Argument3.Memory.BaseRegister);
    m_stmtInsertDisasm->bind(":a3_indexreg", inst->Main.Argument3.Memory.IndexRegister);
    m_stmtInsertDisasm->bind(":a3_scale", inst->Main.Argument3.Memory.Scale);
    m_stmtInsertDisasm->bind(":a3_disp", inst->Main.Argument3.Memory.Displacement);

    m_stmtInsertDisasm->bind(":p_oper", inst->Main.Prefix.OperandSize != 0);
    m_stmtInsertDisasm->bind(":p_addr", inst->Main.Prefix.AddressSize != 0);
    m_stmtInsertDisasm->bind(":p_repne", inst->Main.Prefix.RepnePrefix != 0);
    m_stmtInsertDisasm->bind(":p_rep", inst->Main.Prefix.RepPrefix != 0);
    m_stmtInsertDisasm->bind(":aux_opcode", inst->Aux.opcode);
    m_stmtInsertDisasm->bind(":aux_modrm", inst->Aux.modrm);
    m_stmtInsertDisasm->bind(":aux_sib", inst->Aux.sib);

    m_stmtInsertDisasm->exec();
    m_stmtInsertDisasm->reset();
    m_insts.insert(inst->Eip);
}
