#include "core/autonative.h"
#include "core/script_engine.h"

namespace vspdotnet
{

CREATE_GETTER_FUNCTION(Vector, float, Length, Vector*, obj->Length());
CREATE_GETTER_FUNCTION(Vector, float, Length2D, Vector*, obj->Length2D());
CREATE_GETTER_FUNCTION(Vector, float, LengthSqr, Vector*, obj->LengthSqr());
CREATE_GETTER_FUNCTION(Vector, float, Length2DSqr, Vector*, obj->Length2DSqr());
CREATE_GETTER_FUNCTION(Vector, float, IsZero, Vector*, obj->IsZero());

CREATE_GETTER_FUNCTION(Vector, float, X, Vector*, obj->x);
CREATE_GETTER_FUNCTION(Vector, float, Y, Vector*, obj->y);
CREATE_GETTER_FUNCTION(Vector, float, Z, Vector*, obj->z);

CREATE_SETTER_FUNCTION(Vector, float, X, Vector*, obj->x = value);
CREATE_SETTER_FUNCTION(Vector, float, Y, Vector*, obj->y = value);
CREATE_SETTER_FUNCTION(Vector, float, Z, Vector*, obj->z = value);

Vector* VectorNew(ScriptContext& script_context)
{
  return new Vector();
}

QAngle* AngleNew(ScriptContext& script_context) { return new QAngle(); }

void NativeVectorAngles(ScriptContext& script_context)
{
  auto vec = script_context.GetArgument<Vector*>(0);
  auto pseudoUpVector = script_context.GetArgument<Vector*>(1);
  auto outAngle = script_context.GetArgument<QAngle*>(2);

  if (!pseudoUpVector)
  {
    VectorAngles(*vec, *outAngle);    
  } else
  {
    VectorAngles(*vec, *pseudoUpVector, *outAngle); 
  }

}

REGISTER_NATIVES(vector, {
  ScriptEngine::RegisterNativeHandler("VECTOR_NEW", VectorNew);
  ScriptEngine::RegisterNativeHandler("ANGLE_NEW", AngleNew);

  ScriptEngine::RegisterNativeHandler("VECTOR_SET_X", VectorSetX);
  ScriptEngine::RegisterNativeHandler("VECTOR_SET_Y", VectorSetY);
  ScriptEngine::RegisterNativeHandler("VECTOR_SET_Z", VectorSetZ);

  ScriptEngine::RegisterNativeHandler("VECTOR_GET_X", VectorGetX);
  ScriptEngine::RegisterNativeHandler("VECTOR_GET_Y", VectorGetY);
  ScriptEngine::RegisterNativeHandler("VECTOR_GET_Z", VectorGetZ);

  ScriptEngine::RegisterNativeHandler("VECTOR_ANGLES", NativeVectorAngles);
  ScriptEngine::RegisterNativeHandler("VECTOR_LENGTH", VectorGetLength);
  ScriptEngine::RegisterNativeHandler("VECTOR_LENGTH_2D", VectorGetLength2D);
  ScriptEngine::RegisterNativeHandler("VECTOR_LENGTH_SQR", VectorGetLengthSqr);
  ScriptEngine::RegisterNativeHandler("VECTOR_LENGTH_2D_SQR", VectorGetLength2DSqr);
  ScriptEngine::RegisterNativeHandler("VECTOR_IS_ZERO", VectorGetLengthSqr);


})
}