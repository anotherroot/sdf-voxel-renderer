#type compute
#version 430
layout(local_size_x = 8, local_size_y = 8,local_size_z = 8) in;
layout(rgba8, binding = 0) uniform image3D img_output;

layout( location=1 ) uniform float u_time;  




#define PI 3.141592
#define EPS .005
#define FAR 20.
#define ZERO 0
#define R iResolution.xy
#define T iTime

#define SKULL 1.0
#define TEETH 2.0
#define STONE_I 3.0 
#define STONE_O 4.0 
#define GLOW 5.0
#define BLACK 6.0

//Fabrice - compact rotation
mat2 rot(float x) {return mat2(cos(x), sin(x), -sin(x), cos(x));}


float saturate(float x) {return clamp(x, 0.0, 1.0);}
vec3 saturate(vec3 x) {return clamp(x, vec3(0.0), vec3(1.0));}

//Shane IQ
float n3D(vec3 p) {    
	const vec3 s = vec3(7, 157, 113);
	vec3 ip = floor(p); 
    p -= ip; 
    vec4 h = vec4(0., s.yz, s.y + s.z) + dot(ip, s);
    p = p * p * (3. - 2. * p);
    h = mix(fract(sin(h) * 43758.5453), fract(sin(h + s.x) * 43758.5453), p.x);
    h.xy = mix(h.xz, h.yw, p.y);
    return mix(h.x, h.y, p.z);
}

//Distance functions - IQ
//https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdSphere(vec3 p, float r) {
    return length(p) -  r;    
}

float sdEllipsoid(vec3 p, vec3 r) {
    return (length(p / r) - 1.) * min(min(r.x, r.y), r.z);
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba*h) - r;
}

float sdTorus(vec3 p, vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float sdCappedCylinder(vec3 p, float h, float r) {
    vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

float sdRoundBox(vec3 p, vec3 b, float r) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float sdTriPrism(vec3 p, vec2 h) {
    vec3 q = abs(p);
    return max(q.y-h.y,max(q.x*0.866025+p.z*0.5,-p.z)-h.x*0.5);
}

float sdLink(in vec3 p, in float le, in float r1, in float r2) {
    vec3 q = vec3( p.y, max(abs(p.x)-le,0.0), p.z );
    return length(vec2(length(q.yx)-r1,q.z)) - r2;
}

float sdEqTriangle(vec2 p) {
    const float k = sqrt(3.0);
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0 / k;
    if (p.x + k * p.y > 0.0) p = vec2(p.x - k * p.y, - k * p.x - p.y) / 2.0;
    p.x -= clamp(p.x, -2.0, 0.0);
    return -length(p) * sign(p.y);
}

//mercury
float pModPolar(inout vec2 p, float repetitions) {
    float angle = 2.0 * PI / repetitions,
          a = atan(p.y, p.x) + angle / 2.0,
          r = length(p),
          c = floor(a / angle);
    a = mod(a, angle) - angle / 2.0;
    p = vec2(cos(a), sin(a)) * r;
    // For an odd number of repetitions, fix cell index of the cell in -x direction
    // (cell index would be e.g. -5 and 5 in the two halves of the cell):
    if (abs(c) >= (repetitions / 2.0)) c = abs(c);
    return c;
}

float smin(float a, float b, float k) {
	float h = clamp(0.5 + 0.5 * (b - a) / k, 0., 1.);
	return mix(b, a, h) - k * h * (1. - h);
}

float smax(float a, float b, float k) {
	float h = clamp( 0.5 + 0.5 * (b - a) / k, 0.0, 1.0 );
	return mix(a, b, h) + k * h * (1.0 - h);
}

float pattern(vec2 uv, float size) {
    
    for(int i=0; i<5; ++i) {
		uv *= rot(float(i)+1.7);
        uv = abs(fract(uv/30. + .5) - .5)*30.; //NuSan
		uv -= size;
		size *= 0.5;
	}
    
    float t = length(uv+vec2(0.0, -1.5)) - 0.4;
    t = max(t, -(length(uv+vec2(0.0, -1.5)) - 0.3));
    t = min(t, length(uv+vec2(0.0, -1.5)) - 0.2);
    t = min(t, sign(sdEqTriangle((uv+vec2(0.0,-0.5)) * 1.0)) * sign(sdEqTriangle((uv+vec2(0.0,-0.5)) * 1.3)));
    return step(0.0, t);
}

vec2 nearest(vec2 a, vec2 b) {
    float s = step(a.x, b.x);
    return s*a + (1.0-s)*b;
}

vec2 dfSkull(vec3 p) {
    
    p.yz *= rot(0.2);
    vec3 q = p;
  
    float nz = (n3D(p*2.0) - 0.5) * 0.06;
    //teeth
    q.xz *= rot(0.0981);
    pModPolar(q.xz, 32.0);
    float teeth = sdEllipsoid(q - vec3(0.7, 0.0, 0.0), vec3(0.04, 0.18, 0.06));
    teeth = min(smax(teeth, -sdBox(q - vec3(0.0, -1.0, 0.0), vec3(2.0, 1.0, 2.0)), 0.04), 
                smax(teeth, -sdBox(q - vec3(0.0, 1.0, 0.0), vec3(2.0, 1.0, 2.0)), 0.04));
    teeth = max(teeth, p.z);    

    //symetry
    p.x = abs(p.x);
    
    //skull
    q = p;
    float skull = sdCapsule(p, vec3(0.0, 1.5, 0.6), vec3(0.0, 1.5, 1.0), 1.36+nz);
    skull = smin(skull, sdSphere(p - vec3(0.0, 1.5, 1.0), 1.46), 0.2+nz);
    //roof of mouth
    skull = smin(skull, sdEllipsoid(q - vec3(0.0, 0.2, 0.0), vec3(0.72, 0.8, 0.72)), 0.3);
    skull = max(skull, -sdEllipsoid(q - vec3(0.0, 0.2, 0.0), vec3(0.52, 0.6, 0.52)));
    skull = smax(skull, -sdBox(q - vec3(0.0, -0.9, 0.0), vec3(2.0, 1.0, 2.0)), 0.02);
    
    //jaw socket
    skull = smax(skull, -sdCappedCylinder(p.yxz - vec3(0.0, 0.0, 0.7), 0.5, 2.0), 0.2);
    
    //jaw
    float jaw = sdEllipsoid(q - vec3(0.0, -0.2, 0.0), vec3(0.72, 0.5, 0.7)+nz);
    jaw = max(jaw, -sdEllipsoid(q - vec3(0.0, -0.2, 0.0), vec3(0.65, 0.52, 0.65)));
    jaw = smax(jaw, -sdBox(q - vec3(0.0, 0.9, 0.0), vec3(2.0, 1.0, 2.0)), 0.02);
    q.yz *= rot(-0.2);
    jaw = smin(jaw, sdTorus(q - vec3(0.0, -0.38, -0.2), vec2(0.5, 0.08+nz)), 0.14);
	q.x = abs(q.x);
    jaw = smin(jaw, sdEllipsoid(q - vec3(0.58, -0.36, 0.3), vec3(0.3, 0.14, 0.6)+nz), 0.1);
    jaw = smax(jaw, -sdBox(q - vec3(0.0, 0.0, 1.7), vec3(0.5, 2.0, 2.0)), 0.02);
    jaw = smax(jaw, -sdBox(q - vec3(0.0, 0.0, 2.5), vec3(2.0, 1.0, 2.0)), 0.08);
    q = p;
    q.yz *= rot(-0.2);
    q.xy *= rot(-0.2);
    jaw = smin(jaw, sdEllipsoid(q - vec3(0.8, 0.3, 0.36), vec3(0.1, 0.40, 0.14)+nz), 0.2);
    jaw = smin(jaw, sdCapsule(p, vec3(0.9,0.2,0.3), vec3(1.1,0.5,0.6), 0.08+nz), 0.08);
    skull = min(skull, jaw);
    
    //eyebrow
    skull = smax(skull, -sdCapsule(p, vec3(0.8,0.8,-0.9), vec3(-0.8,0.8,-0.9), 0.3), 0.16);
    //eye socket
    skull = smax(skull, -sdEllipsoid(vec3(abs(p.x),p.y,p.z) - vec3(0.4,0.8,-0.26), vec3(0.4,0.3,0.3)), 0.1);

    //temple
    q = p;
    q.xz *= rot(-0.4);
    q.xy *= rot(-0.3);
    skull = smax(skull, -sdEllipsoid(q - vec3(0.64,1.2,0.8), vec3(0.1+nz,0.6,0.7)), 0.1);
    
    //eye socket
    nz = n3D(p*5.0) * 0.03;
    
    q = p;
    q += vec3(-0.4, -0.8, 0.50);
    q.y *= 1.0 - abs(p.x)*0.3;
    q.z -= abs(q.x*q.x*0.8);
    q.z += q.y*0.2;
    float brow = sdLink(q, 0.34, 0.24+nz, 0.08+nz);
    skull = smin(skull, brow, 0.1);
    q = p;
    q += vec3(-1.14, -0.9, -0.44);
    q.y += q.z*q.z*0.4;
    q.x += q.z*q.z*0.6;
    q.yz *= rot(-0.2);
    skull = smin(skull, sdCapsule(q, vec3(0.0,0.0,-0.66),  vec3(0.0,0.0,0.66), 0.1+nz), 0.1); 
    
    //nose
    skull = smin(skull, sdEllipsoid(p - vec3(0.0, 0.5, -0.3), vec3(0.4,0.3,0.44)), 0.1);    
    float nose = sdCapsule(p, vec3(0.0,0.62,-0.78), vec3(0.0,1.2,-0.5), 0.18 - p.y*0.08 + nz);
    skull = smin(skull, nose, 0.14);
    nz = n3D(p*19.0)*0.05;
    skull = smax(skull, -sdEllipsoid(p - vec3(0.0,0.56,-0.98), vec3(0.34,0.2,0.34)+nz), 0.02);
    
    //nostril
    q = p;
    q += vec3(-0.08, -0.55, 0.7);
    q.yz *= rot(1.4);
    q.xz *= rot(0.523);
    skull = smax(skull, -sdTriPrism(q, vec2(0.14, 0.3)), 0.03);
    
    //cutout teeth
    skull = smax(skull, -teeth, 0.02);
    
    return nearest(vec2(skull, SKULL), vec2(teeth, TEETH));       
}

vec3 dfHalo(vec3 p) {
    
    vec3 q = p.xzy;
    float stoneO = sdCappedCylinder(q, 50.0, 0.2);
    stoneO = max(stoneO, -sdCappedCylinder(q, 3.5, 1.0));
    float stoneI = sdCappedCylinder(q, 3.0, 0.2);
    stoneI = max(stoneI, -sdCappedCylinder(q, 2.0, 1.0));
    
    float glow = sdTorus(p.xzy, vec2(1.9, 0.01));
    glow = min(glow, sdTorus(p.xzy, vec2(3.3, 0.01)));
    
    //ughhh!!!
    /*
    q = p;
    float black = sdCapsule(q, vec3(-3.0,-1.0,-0.5), vec3(3.0,-1.0,-0.5), 0.08);
    q.xy *= rot(PI*2.0/5.0);
    black = min(black, sdCapsule(q, vec3(-3.0,-1.0,-0.5), vec3(3.0,-1.0,-0.5), 0.08));
    q.xy *= rot(PI*2.0/5.0);
    black = min(black, sdCapsule(q, vec3(-3.0,-1.0,-0.5), vec3(3.0,-1.0,-0.5), 0.08));
    q.xy *= rot(PI*2.0/5.0);
    black = min(black, sdCapsule(q, vec3(-3.0,-1.0,-0.5), vec3(3.0,-1.0,-0.5), 0.08));
    q.xy *= rot(PI*2.0/5.0);
    black = min(black, sdCapsule(q, vec3(-3.0,-1.0,-0.5), vec3(3.0,-1.0,-0.5), 0.08));
    //*/
    
    vec2 near = nearest(vec2(stoneI, STONE_I), vec2(stoneO, STONE_O));
    near = nearest(near, vec2(glow, GLOW));
    //near = nearest(near, vec2(black, BLACK));
    
    return vec3(near, glow);
}

float map(vec3 p) {

    vec2 skull = dfSkull(p);
    
    return max(0,min(skull.y,skull.x));
}

vec3 normal(vec3 p) {
    vec2 e = vec2(EPS, 0);
    float d1 = map(p + e.xyy).x, d2 = map(p - e.xyy).x;
    float d3 = map(p + e.yxy).x, d4 = map(p - e.yxy).x;
    float d5 = map(p + e.yyx).x, d6 = map(p - e.yyx).x;
    float d = map(p).x * 2.0;
    return normalize(vec3(d1 - d2, d3 - d4, d5 - d6));
}

//IQ - http://www.iquilezles.org/www/articles/raymarchingdf/raymarchingdf.htm
float AO(vec3 p, vec3 n) {
    float ra = 0., w = 1., d = 0.;
    for (float i = 1.; i < 12.; i += 1.){
        d = i / 5.;
        ra += w * (d - map(p + n * d).x);
        w *= .5;
    }
    return 1. - clamp(ra, 0., 1.);
}

//IQ - https://www.shadertoy.com/view/lsKcDD
float shadow(vec3 ro, vec3 rd, float mint, float tmax) {
	float res = 1.0;
    float t = mint;
    float ph = 1e10;
    
    for (int i = ZERO; i < 32; i++) {
		float h = map(ro + rd * t).x;
        float y = h * h / (2.0 * ph);
        float d = sqrt(h * h - y * y);
        res = min(res, 10.0 * d / max(0.0, t-y));
        ph = h;        
        t += h;
        if (res < 0.0001 || t > tmax) break;
    }
    
    return clamp(res, 0.0, 1.0);
}







void main() {


  ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz+0.01);
  vec3 local = vec3(gl_LocalInvocationID.xyz);

  vec3 uv = vec3((gl_GlobalInvocationID.xyz/vec3(gl_NumWorkGroups.xyz*gl_WorkGroupSize.xyz))*2-1);
  vec3 pos = gl_GlobalInvocationID.xyz-vec3(gl_NumWorkGroups.xyz*gl_WorkGroupSize.xyz)*0.5;
  float d1 = 30.0;
  pos.y+=170;
  pos/=d1;
  vec4 pixel = vec4(1/255.0);
  float dist = map(pos)*d1/sqrt(3.0f);

  float sha = 1;
  if(int(abs(dist))==0){
    /* sha = shadow(pos,vec3(0,1,-0.5),0.05,400)*2; */
    /* sha*=sha; */

  }
  vec3 norm = normal(pos);
  float ao = AO(pos,norm);
  ao*=ao;
  pixel=vec4(vec3(1,0.95,0.8)*0.6*ao*sha,dist/255);


  imageStore(img_output, pixel_coords, pixel);


}

