import os
import shutil
import sys

import numpy as np
import bpy
import bmesh
import json


class NodesUtils:

    @staticmethod
    def collect_all_before(node, outputs):
        if node not in outputs:
            outputs.add(node)
            for input_socket in node.inputs:
                for link in input_socket.links:
                    NodesUtils.collect_all_before(link.from_node, outputs)
        return outputs

    @staticmethod
    def delete_all_before(node_tree, node):
        for node in NodesUtils.collect_all_before(node, set()):
            node_tree.nodes.remove(node)

    @staticmethod
    def backwards_search_for(node, t: type, outputs):
        if node not in outputs:
            if isinstance(node, t):
                outputs.add(node)
            for i in node.inputs:
                NodesUtils.from_socket_backwards_search_for(i, t, outputs)
        return outputs

    @staticmethod
    def from_socket_backwards_search_for(input_socket, t: type, outputs):
        for link in input_socket.links:
            NodesUtils.backwards_search_for(link.from_node, t, outputs)
        return outputs

    @staticmethod
    def find_by_type(node_tree, t: type):
        for node in node_tree.nodes:
            if isinstance(node, t):
                return node

    @staticmethod
    def find_all_by_type(node_tree, t: type):
        return [node for node in node_tree.nodes if isinstance(node, t)]

    @staticmethod
    def new_mat(name):
        mat = bpy.data.materials.new(name=name)
        mat.use_nodes = True
        mat.node_tree.nodes.clear()
        return mat

    @staticmethod
    def remove_all_mats(mesh_object, name=None):
        mesh_object.data.materials.clear()
        if name is not None:
            mat = NodesUtils.new_mat(name)
            mesh_object.data.materials.append(mat)
            return mat

    @staticmethod
    def gen_simple_material(node_tree, filepaths, output_socket=None, shift_x=0, uvs=None):
        ns = node_tree.nodes
        ls = node_tree.links
        bsdf_node = ns.new('ShaderNodeBsdfPrincipled')
        bsdf_node.location = (shift_x, 0)
        bsdf_node.name = 'simple_material_bsdf'
        if output_socket is None:
            output_node = ns.new('ShaderNodeOutputMaterial')
            output_node.location = (shift_x+400, 0)
            output_socket = output_node.inputs['Surface']
        if isinstance(uvs, str):
            uv_node = ns.new('ShaderNodeUVMap')
            uv_node.location = (-900 + shift_x, 0)
            uv_node.uv_map = uvs
            uvs = uv_node
        if isinstance(uvs, bpy.types.ShaderNodeUVMap):
            uvs = uvs.outputs['UV']
        for idx, channel in enumerate(['Base Color', 'Roughness', 'Normal']):
            if channel in filepaths:
                tex_node = ns.new('ShaderNodeTexImage')
                tex_node.name = 'simple_material_'+channel
                tex_node.location = (-600 + shift_x, -(idx - 1) * 300)
                filepath = filepaths[channel]
                if isinstance(filepath, list):
                    filepath = filepath[0]
                if isinstance(filepath, str):
                    filepath = bpy.data.images.load(filepath)
                    filepath.colorspace_settings.name = 'sRGB' if channel == 'Base Color' else 'Non-Color'
                tex_node.image = filepath
                if uvs is not None:
                    ls.new(tex_node.inputs['Vector'], uvs)
                if channel == 'Normal':
                    norm_map_node = ns.new('ShaderNodeNormalMap')
                    norm_map_node.location = (-200 + shift_x, -idx * 200)
                    norm_map_node.name = 'simple_material_normal_map'
                    ls.new(bsdf_node.inputs[channel], norm_map_node.outputs['Normal'])
                    ls.new(norm_map_node.inputs['Color'], tex_node.outputs['Color'])
                else:
                    ls.new(bsdf_node.inputs[channel], tex_node.outputs['Color'])
        ls.new(output_socket, bsdf_node.outputs['BSDF'])

def install_libraries():
    pil_missing = False
    opencv_missing = False
    try:
        import PIL
    except ModuleNotFoundError:
        pil_missing = True
    try:
        import cv2
    except ModuleNotFoundError:
        opencv_missing = True
    if pil_missing or opencv_missing:
        py_exe = sys.executable
        res_path = os.path.realpath(os.path.join(py_exe, "../../lib/site-packages"))
        print("Run the following command as an admin: ")
        print('&"' + py_exe + '" -m pip install pillow opencv-python "--target=' + res_path + '"')


install_libraries()

BOT_ARM_TRANS = [-0.072266 - 0.006897, 0.085937 + 0.009853]
TOP_ARM_TRANS = [0.043945, 0.006836]
RIGHT_LEG_TRANS = -0.014389
BODY_TRANS = [0.0, 0.170266]
LIP_TRANS = [1 / 4 + 1 / 16, -1 / 32]
LEFT_LEG_COLOR = (64 * 256 + 24) * 256 + 126
RIGHT_LEG_COLOR = (42 * 256 + 126) * 256 + 24
BUTT_COLOR = (255 * 256 + 24) * 256 + 255
BOT_ARM_COLOR = (62 * 256 + 21) * 256 + 211
LIP_COLOR = (21 * 256 + 109) * 256 + 211
TOP_ARM_COLOR = (255 * 256 + 0) * 256 + 0
BODY_COLOR = (21 * 256 + 211) * 256 + 91
HEAD_COLOR = (204 * 256 + 162) * 256 + 20
NEW_GP_UV_MAP = 'unified_gp_uv'
NEW_EYES_UV_MAP = 'optimised_eyes_uvs'
UE5_BONE_HIERARCHY = {'pelvis': '', 'spine_01': 'pelvis', 'spine_02': 'spine_01', 'spine_03': 'spine_02',
                      'spine_04': 'spine_03', 'spine_05': 'spine_04', 'neck_01': 'spine_05', 'neck_02': 'neck_01',
                      'head': 'neck_02', 'clavicle_l': 'spine_05', 'upperarm_l': 'clavicle_l',
                      'lowerarm_l': 'upperarm_l', 'lowerarm_twist_02_l': 'lowerarm_l',
                      'lowerarm_twist_01_l': 'lowerarm_l', 'lowerarm_correctiveRoot_l': 'lowerarm_l',
                      'lowerarm_in_l': 'lowerarm_correctiveRoot_l', 'lowerarm_out_l': 'lowerarm_correctiveRoot_l',
                      'lowerarm_fwd_l': 'lowerarm_correctiveRoot_l', 'lowerarm_bck_l': 'lowerarm_correctiveRoot_l',
                      'hand_l': 'lowerarm_l', 'wrist_inner_l': 'hand_l', 'wrist_outer_l': 'hand_l',
                      'index_metacarpal_l': 'hand_l', 'index_01_l': 'index_metacarpal_l', 'index_02_l': 'index_01_l',
                      'index_03_l': 'index_02_l', 'middle_metacarpal_l': 'hand_l', 'middle_01_l': 'middle_metacarpal_l',
                      'middle_02_l': 'middle_01_l', 'middle_03_l': 'middle_02_l', 'thumb_01_l': 'hand_l',
                      'thumb_02_l': 'thumb_01_l', 'thumb_03_l': 'thumb_02_l', 'pinky_metacarpal_l': 'hand_l',
                      'pinky_01_l': 'pinky_metacarpal_l', 'pinky_02_l': 'pinky_01_l', 'pinky_03_l': 'pinky_02_l',
                      'ring_metacarpal_l': 'hand_l', 'ring_01_l': 'ring_metacarpal_l', 'ring_02_l': 'ring_01_l',
                      'ring_03_l': 'ring_02_l', 'weapon_l': 'hand_l', 'upperarm_twist_01_l': 'upperarm_l',
                      'upperarm_twistCor_01_l': 'upperarm_twist_01_l', 'upperarm_twist_02_l': 'upperarm_l',
                      'upperarm_tricep_l': 'upperarm_twist_02_l', 'upperarm_bicep_l': 'upperarm_twist_02_l',
                      'upperarm_twistCor_02_l': 'upperarm_twist_02_l', 'upperarm_correctiveRoot_l': 'upperarm_l',
                      'upperarm_bck_l': 'upperarm_correctiveRoot_l', 'upperarm_fwd_l': 'upperarm_correctiveRoot_l',
                      'upperarm_in_l': 'upperarm_correctiveRoot_l', 'upperarm_out_l': 'upperarm_correctiveRoot_l',
                      'clavicle_out_l': 'clavicle_l', 'clavicle_scap_l': 'clavicle_l', 'clavicle_r': 'spine_05',
                      'upperarm_r': 'clavicle_r', 'lowerarm_r': 'upperarm_r', 'lowerarm_twist_02_r': 'lowerarm_r',
                      'lowerarm_twist_01_r': 'lowerarm_r', 'lowerarm_correctiveRoot_r': 'lowerarm_r',
                      'lowerarm_out_r': 'lowerarm_correctiveRoot_r', 'lowerarm_in_r': 'lowerarm_correctiveRoot_r',
                      'lowerarm_fwd_r': 'lowerarm_correctiveRoot_r', 'lowerarm_bck_r': 'lowerarm_correctiveRoot_r',
                      'hand_r': 'lowerarm_r', 'wrist_inner_r': 'hand_r', 'wrist_outer_r': 'hand_r',
                      'pinky_metacarpal_r': 'hand_r', 'pinky_01_r': 'pinky_metacarpal_r', 'pinky_02_r': 'pinky_01_r',
                      'pinky_03_r': 'pinky_02_r', 'ring_metacarpal_r': 'hand_r', 'ring_01_r': 'ring_metacarpal_r',
                      'ring_02_r': 'ring_01_r', 'ring_03_r': 'ring_02_r', 'middle_metacarpal_r': 'hand_r',
                      'middle_01_r': 'middle_metacarpal_r', 'middle_02_r': 'middle_01_r', 'middle_03_r': 'middle_02_r',
                      'index_metacarpal_r': 'hand_r', 'index_01_r': 'index_metacarpal_r', 'index_02_r': 'index_01_r',
                      'index_03_r': 'index_02_r', 'thumb_01_r': 'hand_r', 'thumb_02_r': 'thumb_01_r',
                      'thumb_03_r': 'thumb_02_r', 'weapon_r': 'hand_r', 'upperarm_twist_01_r': 'upperarm_r',
                      'upperarm_twistCor_01_r': 'upperarm_twist_01_r', 'upperarm_twist_02_r': 'upperarm_r',
                      'upperarm_tricep_r': 'upperarm_twist_02_r', 'upperarm_bicep_r': 'upperarm_twist_02_r',
                      'upperarm_twistCor_02_r': 'upperarm_twist_02_r', 'upperarm_correctiveRoot_r': 'upperarm_r',
                      'upperarm_bck_r': 'upperarm_correctiveRoot_r', 'upperarm_in_r': 'upperarm_correctiveRoot_r',
                      'upperarm_fwd_r': 'upperarm_correctiveRoot_r', 'upperarm_out_r': 'upperarm_correctiveRoot_r',
                      'clavicle_out_r': 'clavicle_r', 'clavicle_scap_r': 'clavicle_r', 'clavicle_pec_r': 'spine_05',
                      'spine_04_latissimus_l': 'spine_05', 'spine_04_latissimus_r': 'spine_05',
                      'clavicle_pec_l': 'spine_05', 'thigh_r': 'pelvis', 'calf_r': 'thigh_r', 'foot_r': 'calf_r',
                      'ball_r': 'foot_r', 'ankle_fwd_r': 'foot_r', 'ankle_bck_r': 'foot_r', 'calf_twist_02_r': 'calf_r',
                      'calf_twistCor_02_r': 'calf_twist_02_r', 'calf_twist_01_r': 'calf_r',
                      'calf_correctiveRoot_r': 'calf_r', 'calf_kneeBack_r': 'calf_correctiveRoot_r',
                      'calf_knee_r': 'calf_correctiveRoot_r', 'thigh_twist_01_r': 'thigh_r',
                      'thigh_twistCor_01_r': 'thigh_twist_01_r', 'thigh_twist_02_r': 'thigh_r',
                      'thigh_twistCor_02_r': 'thigh_twist_02_r', 'thigh_correctiveRoot_r': 'thigh_r',
                      'thigh_fwd_r': 'thigh_correctiveRoot_r', 'thigh_bck_r': 'thigh_correctiveRoot_r',
                      'thigh_out_r': 'thigh_correctiveRoot_r', 'thigh_in_r': 'thigh_correctiveRoot_r',
                      'thigh_bck_lwr_r': 'thigh_correctiveRoot_r', 'thigh_fwd_lwr_r': 'thigh_correctiveRoot_r',
                      'thigh_l': 'pelvis', 'calf_l': 'thigh_l', 'foot_l': 'calf_l', 'ball_l': 'foot_l',
                      'ankle_bck_l': 'foot_l', 'ankle_fwd_l': 'foot_l', 'calf_twist_02_l': 'calf_l',
                      'calf_twistCor_02_l': 'calf_twist_02_l', 'calf_twist_01_l': 'calf_l',
                      'calf_correctiveRoot_l': 'calf_l', 'calf_kneeBack_l': 'calf_correctiveRoot_l',
                      'calf_knee_l': 'calf_correctiveRoot_l', 'thigh_twist_01_l': 'thigh_l',
                      'thigh_twistCor_01_l': 'thigh_twist_01_l', 'thigh_twist_02_l': 'thigh_l',
                      'thigh_twistCor_02_l': 'thigh_twist_02_l', 'thigh_correctiveRoot_l': 'thigh_l',
                      'thigh_bck_l': 'thigh_correctiveRoot_l', 'thigh_fwd_l': 'thigh_correctiveRoot_l',
                      'thigh_out_l': 'thigh_correctiveRoot_l', 'thigh_bck_lwr_l': 'thigh_correctiveRoot_l',
                      'thigh_in_l': 'thigh_correctiveRoot_l', 'thigh_fwd_lwr_l': 'thigh_correctiveRoot_l',
                      'interaction': '', 'center_of_mass': '', 'IK_PT_Foot_L': '', 'IK_PT_Foot_R': '',
                      'IK_PT_Hand_L': '', 'IK_PT_Hand_R': '', 'ControllersPanel': '', 'LFootIKFK': 'ControllersPanel',
                      'RFootIKFK': 'ControllersPanel', 'LHandIKFK': 'ControllersPanel', 'RHandIKFK': 'ControllersPanel',
                      'BalanceAutoManual': 'ControllersPanel', 'Base': '', 'FootRoot': 'Base', 'IK_Foot_L': 'FootRoot',
                      'IK_Foot_Rotation_L': 'IK_Foot_L', 'Foot_Offset_L': 'IK_Foot_Rotation_L',
                      'IK_Foot_Solver_L': 'IK_Foot_Rotation_L', 'IK_LegLength_Target_L': 'IK_Foot_Solver_L',
                      'IK_Foot_R': 'FootRoot', 'IK_Foot_Rotation_R': 'IK_Foot_R', 'Foot_Offset_R': 'IK_Foot_Rotation_R',
                      'IK_Foot_Solver_R': 'IK_Foot_Rotation_R', 'IK_LegLength_Target_R': 'IK_Foot_Solver_R',
                      'HandRoot': 'Base', 'IK_Hand_L': 'HandRoot', 'IK_Hand_Solver_L': 'IK_Hand_L',
                      'IK_ArmLength_Target_L': 'IK_Hand_Solver_L', 'IK_Hand_R': 'HandRoot',
                      'IK_Hand_Solver_R': 'IK_Hand_R', 'IK_ArmLength_Target_R': 'IK_Hand_Solver_R',
                      'TorsoRotation': 'Base', 'PelvisControl': 'TorsoRotation', 'Pelvis': 'PelvisControl',
                      'Spine_01': 'Pelvis', 'Spine_02': 'Spine_01', 'Spine_03': 'Spine_02', 'Spine_04': 'Spine_03',
                      'Spine_05': 'Spine_04', 'Neck_01': 'Spine_05', 'Neck_02': 'Neck_01', 'Head': 'Neck_02',
                      'Clavicle_L': 'Spine_05', 'UpperArm_L': 'Clavicle_L', 'LowerArm_L': 'UpperArm_L',
                      'Hand_L': 'LowerArm_L', 'Ring_L': 'Hand_L', 'Index_Metacarpal_L': 'Hand_L',
                      'Index_01_L': 'Index_Metacarpal_L', 'Index_02_L': 'Index_01_L', 'Index_03_L': 'Index_02_L',
                      'Middle_Metacarpal_L': 'Hand_L', 'Middle_01_L': 'Middle_Metacarpal_L',
                      'Middle_02_L': 'Middle_01_L', 'Middle_03_L': 'Middle_02_L', 'Thumb_Metacarpal_L': 'Hand_L',
                      'Thumb_01_L': 'Thumb_Metacarpal_L', 'Thumb_02_L': 'Thumb_01_L', 'Ring_Metacarpal_L': 'Hand_L',
                      'Ring_01_L': 'Ring_Metacarpal_L', 'Ring_02_L': 'Ring_01_L', 'Ring_03_L': 'Ring_02_L',
                      'Pinky_Metacarpal_L': 'Hand_L', 'Pinky_01_L': 'Pinky_Metacarpal_L', 'Pinky_02_L': 'Pinky_01_L',
                      'Pinky_03_L': 'Pinky_02_L', 'Fingers_L': 'Hand_L', 'Thumb_L': 'Hand_L', 'Index_L': 'Hand_L',
                      'Middle_L': 'Hand_L', 'Pinky_L': 'Hand_L', 'Weapon_Socket_L': 'Hand_L', 'Clavicle_R': 'Spine_05',
                      'UpperArm_R': 'Clavicle_R', 'LowerArm_R': 'UpperArm_R', 'Hand_R': 'LowerArm_R',
                      'Index_Metacarpal_R': 'Hand_R', 'Index_01_R': 'Index_Metacarpal_R', 'Index_02_R': 'Index_01_R',
                      'Index_03_R': 'Index_02_R', 'Middle_Metacarpal_R': 'Hand_R', 'Middle_01_R': 'Middle_Metacarpal_R',
                      'Middle_02_R': 'Middle_01_R', 'Middle_03_R': 'Middle_02_R', 'Thumb_Metacarpal_R': 'Hand_R',
                      'Thumb_01_R': 'Thumb_Metacarpal_R', 'Thumb_02_R': 'Thumb_01_R', 'Ring_Metacarpal_R': 'Hand_R',
                      'Ring_01_R': 'Ring_Metacarpal_R', 'Ring_02_R': 'Ring_01_R', 'Ring_03_R': 'Ring_02_R',
                      'Pinky_Metacarpal_R': 'Hand_R', 'Pinky_01_R': 'Pinky_Metacarpal_R', 'Pinky_02_R': 'Pinky_01_R',
                      'Pinky_03_R': 'Pinky_02_R', 'Ring_R': 'Hand_R', 'Pinky_R': 'Hand_R', 'Thumb_R': 'Hand_R',
                      'Index_R': 'Hand_R', 'Fingers_R': 'Hand_R', 'Middle_R': 'Hand_R', 'Weapon_Socket_R': 'Hand_R',
                      'IK_ArmLenght_L': 'Spine_05', 'Shoulder_Target_L': 'IK_ArmLenght_L', 'IK_ArmLength_R': 'Spine_05',
                      'Shoulder_Target_R': 'IK_ArmLength_R', 'IK_LegLenght_L': 'Spine_05', 'IK_LegLenght_R': 'Spine_05',
                      'Tigh_L': 'Pelvis', 'Calf_L': 'Tigh_L', 'Foot_L': 'Calf_L', 'Ball_L': 'Foot_L',
                      'Tigh_R': 'Pelvis', 'Calf_R': 'Tigh_R', 'Foot_R': 'Calf_R', 'Ball_R': 'Foot_R',
                      'PelvisHeightOffset': 'PelvisControl', 'TorsoControl': 'PelvisControl',
                      'ChestControl': 'TorsoControl', 'HeadControl': 'ChestControl', 'ChestRotation': 'TorsoRotation',
                      'AimTarget': '', 'ik_foot_root': '', 'ik_foot_l': 'ik_foot_root', 'ik_foot_r': 'ik_foot_root',
                      'ik_hand_root': '', 'ik_hand_gun': 'ik_hand_root', 'ik_hand_l': 'ik_hand_gun',
                      'ik_hand_r': 'ik_hand_gun'}
UE5_IK_BONES = {
    'ik_foot_root': '',
    'ik_foot_l': 'foot_l',
    'ik_foot_r': 'foot_r',
    'ik_hand_root': '',
    'ik_hand_gun': 'hand_r',
    'ik_hand_l': 'hand_l',
    'ik_hand_r': 'hand_r',
    'center_of_mass': ''
}
DAZ_G9_TO_UE5_BONES = {
    'r_toes': 'ball_r',
    'l_toes': 'ball_l',
    'r_foot': 'foot_r',
    'l_foot': 'foot_l',
    'r_shin': 'calf_r',
    'l_shin': 'calf_l',
    'l_thigh': 'thigh_l',
    'r_thigh': 'thigh_r',
    'hip': 'pelvis',
    'pelvis': 'spine_01',
    'spine1': 'spine_02',
    'spine2': 'spine_03',
    'spine3': 'spine_04',
    'l_pectoral': 'clavicle_pec_l',
    'r_pectoral': 'clavicle_pec_r',
    'spine4': 'spine_05',
    'l_shoulder': 'clavicle_l',
    'r_shoulder': 'clavicle_r',
    'l_upperarm': 'upperarm_l',
    'r_upperarm': 'upperarm_r',
    'l_forearm': 'lowerarm_l',
    'r_forearm': 'lowerarm_r',
    'l_hand': 'hand_l',
    'r_hand': 'hand_r',
    'l_thumb1': 'thumb_01_l',
    'r_thumb1': 'thumb_01_r',
    'l_thumb2': 'thumb_02_l',
    'r_thumb2': 'thumb_02_r',
    'l_thumb3': 'thumb_03_l',
    'r_thumb3': 'thumb_03_r',
    'l_index1': 'index_01_l',
    'r_index1': 'index_01_r',
    'l_index2': 'index_02_l',
    'r_index2': 'index_02_r',
    'l_index3': 'index_03_l',
    'r_index3': 'index_03_r',
    'l_mid1': 'middle_01_l',
    'r_mid1': 'middle_01_r',
    'l_mid2': 'middle_02_l',
    'r_mid2': 'middle_02_r',
    'l_mid3': 'middle_03_l',
    'r_mid3': 'middle_03_r',
    'l_ring1': 'ring_01_l',
    'r_ring1': 'ring_01_r',
    'l_ring2': 'ring_02_l',
    'r_ring2': 'ring_02_r',
    'l_ring3': 'ring_03_l',
    'r_ring3': 'ring_03_r',
    'l_pinky1': 'pinky_01_l',
    'r_pinky1': 'pinky_01_r',
    'l_pinky2': 'pinky_02_l',
    'r_pinky2': 'pinky_02_r',
    'l_pinky3': 'pinky_03_l',
    'r_pinky3': 'pinky_03_r',
    'neck1': 'neck_01',
    'neck2': 'neck_02',
    'head': 'head',
}
BUTT_EXTRA_BONES = {
    'l_butt_01': ((0.0750763937830925, -0.07929152250289917, 0.004994665738195181), (0.07388557493686676, -0.06475628912448883, 0.07164420187473297)),
    'l_butt_02': ((0.0, 0.0, 0.0), (0.00088588084327057, 0.034035515040159225, 0.001118007581681013)),
    'l_butt_03': ((0.0, 0.0, 0.0), (-0.0003263086546212435, 0.022132765501737595, 0.0002884981222450733)),
    'r_butt_01': ((-0.07502999901771545, -0.07925647497177124, 0.004993385635316372), (-0.07388557493686676, -0.06475628912448883, 0.07164420187473297)),
    'r_butt_02': ((0.0, 0.0, 0.0), (-0.0008686043438501656, 0.03403671085834503, 0.001094963401556015)),
    'r_butt_03': ((0.0, 0.0, 0.0), (0.0003263087710365653, 0.022132769227027893, 0.00028849835507571697))
}
def select_bone(bone):
    bone.select = True
    bone.select_head = True
    bone.select_tail = True


def find_body_rig():
    o = bpy.data.objects[0]
    while o.parent is not None:
        o = o.parent
    return o

def select_object(obj):
    if bpy.context.view_layer.objects.active is not None:
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='OBJECT')

class DazOptimizer:

    def __init__(self, workdir=None, name=None):
        if workdir is None:
            workdir = os.path.dirname(bpy.data.filepath)
        if name is None:
            name = os.path.basename(bpy.data.filepath)
            name = name[:name.rindex('.')]
        self.name = name
        self.workdir = workdir
        self.body_mesh = None
        self.body_rig = None

    def gold_palace_dir(self):
        return os.path.join(self.workdir, "textures/original/meipex/m_goldenpalace/g9")

    def us_mask_path(self):
        return os.path.join(self.workdir, "uv_region_mask.png")

    def textures_dir(self):
        return os.path.join(self.workdir, "textures")

    def genesis_dir(self):
        return os.path.join(self.workdir, "textures/original/daz/characters/genesis9")

    def get_uv_mask(self):
        from PIL import Image
        uv_region_mask = np.array(Image.open(self.us_mask_path()), dtype=np.uint32)
        uv_region_mask = (uv_region_mask[:, :, 0] * 256 + uv_region_mask[:, :, 1]) * 256 + uv_region_mask[:, :, 2]
        return uv_region_mask

    def find_body(self):
        self.body_rig = find_body_rig()
        self.body_mesh = bpy.data.objects[self.body_rig.name + ' Mesh']

    def get_body_mesh(self):
        if self.body_mesh is None:
            self.find_body()
        return self.body_mesh

    def get_body_rig(self):
        if self.body_rig is None:
            self.find_body()
        return self.body_rig

    def get_eyes_mesh(self):
        return bpy.data.objects['Genesis 9 Eyes Mesh']

    def get_base_uv_layer(self, layer_name='Base Multi UDIM'):
        return self.get_body_mesh().data.uv_layers[layer_name]

    def get_base_uv_layer_np(self, layer_name='Base Multi UDIM'):
        return np.array([v.uv for v in self.get_base_uv_layer(layer_name=layer_name).data])

    def get_base_uv_layer_selection_np(self):
        return np.array([v.select for v in self.get_base_uv_layer().data], dtype=bool)

    def update_base_uv_layer(self, base_layer_np: np.ndarray):
        for v, new_uv in zip(self.get_base_uv_layer().data, base_layer_np):
            v.uv = new_uv

    def get_concat_image_path(self, map_type):
        return os.path.join(self.workdir, self.name + '_' + map_type + '.png')

    def get_simplified_eyes_image_path(self, map_type):
        return os.path.join(self.workdir, self.name + '_' + map_type + '_eyes.png')

    def optimize_eyes(self):

        EYES_M = self.get_eyes_mesh()
        select_object(EYES_M)

        # uv_layer = EYES_M.data.uv_layers.active
        # uvs = np.array([v.uv for v in uv_layer.data], dtype=bool)
        # uvs[:, y] < 0.5
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        for v in bm.verts:
            v.select = False

        for face in bm.faces:
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                dist = np.linalg.norm(uv % 0.5 - 0.25)
                if uv[1] < 0.5 or dist > 0.24 or dist < 0.038:
                    loop.vert.select = True
        for v in bm.verts:
            if v.select:
                bm.verts.remove(v)

        def select_loop(center):
            for face in bm.faces:
                for loop in face.loops:
                    loop_uv = loop[uv_layer]
                    uv = np.array(loop_uv.uv)
                    dist = np.linalg.norm(uv - center)
                    loop.vert.select_set(dist < 0.044)

        bmesh.update_edit_mesh(me)

        select_loop([0.25, 0.75])
        bpy.ops.mesh.edge_face_add()
        select_loop([0.75, 0.75])
        bpy.ops.mesh.edge_face_add()

        bmesh.update_edit_mesh(me)

        bpy.ops.object.mode_set(mode='OBJECT')
        #

    def merge_all_rigs(self):
        body_rig = self.get_body_rig()
        select_object(body_rig)
        meshes = []
        for o in bpy.data.objects:
            if isinstance(o.data, bpy.types.Armature):
                if 'hair' in o.name.lower():
                    if o.parent != body_rig:
                        o.parent = body_rig
                    o.hide_viewport = True
                    o.hide_set(True)
                else:
                    o.hide_viewport = False
                    o.hide_render = False
                    o.hide_set(False)
                    # o.data.hide_set(False)
                    o.select_set(True)
                meshes.extend(o.children)
        bpy.ops.daz.merge_rigs()
        for o in bpy.data.objects:
            o.hide_viewport = False
            o.hide_render = False
            o.hide_set(False)
        for mesh in meshes:
            for mod in mesh.modifiers:
                if isinstance(mod, bpy.types.ArmatureModifier):
                    if mod.object is None:
                        mod.object = body_rig
            if mesh.parent is None:
                mesh.parent = body_rig

    def simplify_eyes_material(self):
        from PIL import Image
        textures = self.find_eyes_textures()
        EYES_M = self.get_eyes_mesh()
        mat = NodesUtils.remove_all_mats(EYES_M, "Eyes")


        class EyeMapType:
            def __init__(self, sclera:bpy.types.Image, iris:bpy.types.Image, simplified_texture:str):
                self.sclera = sclera
                self.iris = iris
                self.simplified_texture = simplified_texture
            def join(self):
                iris = Image.open(bpy.path.abspath(self.iris.filepath))
                sclera = Image.open(bpy.path.abspath(self.sclera.filepath))
                iris = np.array(iris)
                sclera = np.array(sclera)
                if iris.dtype.kind != 'f':
                    iris = iris / np.float32(255)
                if sclera.dtype.kind != 'f':
                    sclera = sclera / np.float32(255)
                iris_rgb = iris[:, :, :3]
                iris_a = iris[:, :, 3]

                sclera_h, sclera_w = sclera.shape[:2]
                sclera_half = sclera[:sclera_h//2].T
                sclera_half *= (1 - iris_a.T)
                sclera_half += iris_rgb.T * iris_a.T
                sclera_half = sclera_half.T
                sclera[sclera_h // 2:] = sclera_half
                sclera = Image.fromarray((sclera*255).astype(np.uint8))
                sclera.save(self.simplified_texture)
                return iris_a


        body_part_filepaths = {}
        for channel, images in textures.items():
            iris_img = None
            sclera_img = None
            simplified_texture = self.get_simplified_eyes_image_path(channel)
            for image in images:
                if 'iris' in image.filepath:
                    iris_img = image
                elif 'sclera' in image.filepath:
                    sclera_img = image
            if iris_img is not None and sclera_img is not None:
                eye_map = EyeMapType(sclera_img, iris_img, simplified_texture)
                eye_map.join()
                body_part_filepaths[channel] = eye_map.simplified_texture




        NodesUtils.gen_simple_material(mat.node_tree, body_part_filepaths)

    def separate_iris_uvs(self):
        EYES_M = self.get_eyes_mesh()
        old_uv_layer = EYES_M.data.uv_layers.active
        new_uv_layer = EYES_M.data.uv_layers.new(name=NEW_EYES_UV_MAP)
        new_uv_layer.active = True
        new_uv_layer.active_render = True
        select_object(EYES_M)
        bpy.ops.object.mode_set(mode='EDIT')

        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        # for v in bm.verts:
        #    v.select_set(False)
        iris_uv_radius = 0.86577 - 0.75 + 0.001

        for face in bm.faces:
            full_loop = True
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                is_iris = np.linalg.norm(np.mod(uv, 0.5) - 0.25) < iris_uv_radius
                full_loop = full_loop and is_iris
            face.select_set(full_loop)

        # bm.select_mode = {'VERT', 'EDGE', 'FACE'}
        bm.select_flush_mode()
        # bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        bpy.ops.uv.select_all(action='SELECT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.uv.select_split()

        bpy.ops.object.mode_set(mode='OBJECT')
        new_uv_layer_np = np.array([v.uv for v in EYES_M.data.uv_layers.active.data])
        # old_uv_layer_np = np.array([v.uv for v in old_uv_layer.data])
        selection = np.array([v.select for v in EYES_M.data.uv_layers.active.data], dtype=bool)
        new_uv_layer_np[selection, 1] -= 0.5
        for v, new_uv in zip(EYES_M.data.uv_layers.active.data, new_uv_layer_np):
            v.uv = new_uv
        # += [0.043945, 0.006836] # top arm
        # += [-0.072266 , 0.085937] # obttom arm
        # += [0.008526, 0.019377] # torso
        # *= 0.25# nails
        # -= 0.5 # nails

    def find_eyes_textures(self)->{str:[bpy.types.Image]}:
        EYES_M = self.get_eyes_mesh()
        all_eyes_textures = {}
        for mat in EYES_M.data.materials:
            output_node = NodesUtils.find_by_type(mat.node_tree, bpy.types.ShaderNodeOutputMaterial)
            if output_node is not None:
                for bsdf in NodesUtils.from_socket_backwards_search_for(output_node.inputs['Surface'], bpy.types.ShaderNodeBsdfPrincipled, set()):
                    for channel in ['Base Color', 'Roughness', 'Normal']:
                        all_eyes_textures[channel] = []
                        for img_node in NodesUtils.from_socket_backwards_search_for(bsdf.inputs[channel],(bpy.types.ShaderNodeTexImage, bpy.types.ShaderNodeGroup), set()):
                            if isinstance(img_node, bpy.types.ShaderNodeGroup):
                                for img_node in NodesUtils.find_all_by_type(img_node.node_tree, bpy.types.ShaderNodeTexImage):
                                    all_eyes_textures[channel].append(img_node.image)
                            else:
                                all_eyes_textures[channel].append(img_node.image)
        print("eyes=", json.dumps({k: [v3.filepath for v3 in v] for k, v in all_eyes_textures.items()},indent=2))
        return all_eyes_textures

    def find_body_parts_textures(self):
        BODY_M = self.get_body_mesh()

        all_filepaths: {str: {str: [bpy.types.Image]}} = {}
        for mat in BODY_M.data.materials:
            output_node = NodesUtils.find_by_type(mat.node_tree, bpy.types.ShaderNodeOutputMaterial)
            body_part = mat.name.rstrip('0123456789-_.')
            body_part_filepaths = all_filepaths[body_part] = {'Base Color': set(), 'Roughness': set(), 'Normal': set()}
            if output_node is not None:
                for bsdf in NodesUtils.from_socket_backwards_search_for(output_node.inputs['Surface'], (bpy.types.ShaderNodeBsdfPrincipled, bpy.types.ShaderNodeGroup), set()):
                    if isinstance(bsdf, bpy.types.ShaderNodeBsdfPrincipled):
                        for channel in ['Base Color', 'Roughness', 'Normal']:
                            for img_node in NodesUtils.from_socket_backwards_search_for(bsdf.inputs[channel],
                                                                                        bpy.types.ShaderNodeTexImage,
                                                                                        set()):
                                body_part_filepaths[channel].add(img_node.image)
                    elif bsdf.node_tree.name == 'DAZ Dual Lobe PBR':
                        for img_node in NodesUtils.from_socket_backwards_search_for(bsdf.inputs['Roughness 1'],
                                                                                    bpy.types.ShaderNodeTexImage,
                                                                                    set()):
                            body_part_filepaths['Roughness'].add(img_node.image)
                        for img_node in NodesUtils.from_socket_backwards_search_for(bsdf.inputs['Roughness 2'],
                                                                                    bpy.types.ShaderNodeTexImage,
                                                                                    set()):
                            body_part_filepaths['Roughness'].add(img_node.image)
                        for img_node in NodesUtils.from_socket_backwards_search_for(bsdf.inputs['Normal'],
                                                                                    bpy.types.ShaderNodeTexImage,
                                                                                    set()):
                            body_part_filepaths['Normal'].add(img_node.image)
        for body_part_name, body_part_filepaths in all_filepaths.items():
            occurrences = {}
            filenames = []
            for channel in body_part_filepaths.values():
                if len(channel)==1:
                    first = next(iter(channel))
                    filepath = first.filepath
                    filenames.append(filepath)
            lcp = os.path.commonprefix(filenames)
            for channel in body_part_filepaths.values():
                for image in channel:
                    if image not in occurrences:
                        occurrences[image] = 0
                    occurrences[image] += 1 + len(os.path.commonprefix([image.filepath, lcp]))

            for channel in body_part_filepaths:
                body_part_filepaths[channel] = list(sorted(body_part_filepaths[channel], key=lambda x: -occurrences[x]))
        print(json.dumps({k: {k2: [v3.filepath for v3 in v2] for k2, v2 in v.items()} for k, v in all_filepaths.items()}, indent=2))
        return all_filepaths

    def simplify_materials(self):
        BODY_M = self.get_body_mesh()
        all_filepaths = self.find_body_parts_textures()



        mats = list(BODY_M.data.materials)
        if 'BreastacularG9 Mesh' in bpy.data.objects:
            mats.extend(bpy.data.objects['BreastacularG9 Mesh'].data.materials)
        for mat in mats:
            body_part = mat.name.rstrip('0123456789-_.')
            body_part_filepaths = all_filepaths[body_part]
            mat.node_tree.nodes.clear()
            NodesUtils.gen_simple_material(mat.node_tree, body_part_filepaths)

        body_part_filepaths = all_filepaths['Body']
        if 'GoldenPalace_G9 Mesh' in bpy.data.objects:
            GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
            for mat in GOLD_PAL_M.data.materials:
                output_node = NodesUtils.find_by_type(mat.node_tree, bpy.types.ShaderNodeOutputMaterial)
                tail = output_node.inputs['Surface'].links[0].from_node
                while isinstance(tail, bpy.types.ShaderNodeGroup) and tail.node_tree.name.startswith('GoldenPalaceG9_Shell_'):
                    output_node = tail
                    tail = output_node.inputs['BSDF'].links[0].from_node
                out_socket = output_node.inputs['BSDF']
                NodesUtils.delete_all_before(mat.node_tree, out_socket.links[0].from_node)
                NodesUtils.gen_simple_material(mat.node_tree, body_part_filepaths, out_socket, shift_x=output_node.location[0]-300,uvs='Default UVs')




    def concat_textures(self):
        from PIL import Image
        import re
        import cv2

        all_filepaths = self.find_body_parts_textures()

        head_filepaths = {}
        arms_filepaths = {}
        legs_filepaths = {}
        nails_filepaths = {}
        body_filepaths = {}
        mouth_filepaths = {}
        eyes_filepaths = {}
        gp_filepaths = {}
        for body_part, body_part_filepaths in all_filepaths.items():
            body_part = body_part.lower()
            if 'head' in body_part:
                head_filepaths = body_part_filepaths
            elif 'arms' in body_part:
                arms_filepaths = body_part_filepaths
            elif 'body' in body_part:
                body_filepaths = body_part_filepaths
            elif 'legs' in body_part:
                legs_filepaths = body_part_filepaths
            elif 'nails' in body_part:
                nails_filepaths = body_part_filepaths
            elif 'mouth' in body_part:
                mouth_filepaths = body_part_filepaths
            elif 'eyes' in body_part:
                eyes_filepaths = body_part_filepaths
            elif body_part.startswith('gp_'):
                gp_filepaths = body_part_filepaths
        uv_region_mask = self.get_uv_mask()
        MASK_TILE_SIZE = uv_region_mask.shape[0] // 2
        arms_region_mask = uv_region_mask[:MASK_TILE_SIZE, MASK_TILE_SIZE:]
        body_region_mask = uv_region_mask[MASK_TILE_SIZE:, MASK_TILE_SIZE:]
        legs_region_mask = uv_region_mask[:MASK_TILE_SIZE, :MASK_TILE_SIZE]
        head_region_mask = uv_region_mask[MASK_TILE_SIZE:, :MASK_TILE_SIZE]

        def open_img(filepaths, map_type, resize=None):
            fp = filepaths[map_type]
            if isinstance(fp, list):
                fp = fp[0]
            if isinstance(fp, bpy.types.Image):
                fp = bpy.path.abspath(fp.filepath)
            tile = Image.open(fp)
            tile = np.array(tile)
            print("Reading ",fp," of size ", tile.shape," and type ", tile.dtype)
            if map_type == "Roughness" and tile.ndim>2 and tile.shape[2]>1:
                tile = np.average(tile, axis=2)
                tile = tile.astype(np.uint8)
                print("Converted to greyscale ", tile.shape, " ",tile.dtype)
            if resize is not None:
                tile = cv2.resize(tile, [s4, s4])
                print("Resized to ", tile.shape, " ", tile.dtype)
            return tile

        # from matplotlib import pyplot as plt
        for map_type in ["Base Color", "Roughness", "Normal"]:
            print("map_type=", map_type)
            head_tile = open_img(head_filepaths, map_type)
            body_tile = open_img(body_filepaths, map_type)
            arms_tile = open_img(arms_filepaths, map_type)
            legs_tile = open_img(legs_filepaths, map_type)

            s = legs_tile.shape[0]
            s2 = s * 2
            s4 = s // 4
            s8 = s // 8
            mouth_tile = None
            if map_type in mouth_filepaths and len(mouth_filepaths[map_type]) > 0:
                mouth_tile = open_img(mouth_filepaths, map_type,  [s4, s4])
            eyes_tile = None
            if map_type in eyes_filepaths and len(eyes_filepaths[map_type])>0:
                eyes_tile = open_img(eyes_filepaths, map_type,  [s4, s4])
            nails_tile = None
            if map_type in nails_filepaths and len(nails_filepaths[map_type])>0:
                nails_tile = open_img(nails_filepaths, map_type,  [s4, s4])
            gp_tile = None
            if map_type in gp_filepaths and len(gp_filepaths[map_type])>0:
                gp_tile = open_img(gp_filepaths, map_type, [s4, s4])

            def shift_img(img: np.ndarray, y0, y1, x0, x1, mask: np.ndarray, translation: [float, float], hflip=False):
                shape = [s2, s2, legs_tile.shape[2]] if len(legs_tile.shape) > 2 else [s2, s2]
                new_img = np.zeros(shape, dtype=legs_tile.dtype)
                if hflip:
                    mask = np.flipud(mask)
                    img = np.flipud(img)
                new_img[y0:y1, x0:x1][mask] = img[mask]
                x, y = np.int32(np.array(translation) * s2)
                if x != 0 or y != 0:
                    new_img = np.roll(new_img, [-y, x], axis=[0, 1])
                return new_img

            # Textures are concatenated as follows:
            #   Legs | Arms
            #  ------+-----
            #   Head | Body
            packed = shift_img(arms_tile, 0, s, s, s2, arms_region_mask == BOT_ARM_COLOR, BOT_ARM_TRANS)
            packed += shift_img(arms_tile, 0, s, s, s2, arms_region_mask == TOP_ARM_COLOR, TOP_ARM_TRANS)
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == LEFT_LEG_COLOR, [0, 0])
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == RIGHT_LEG_COLOR, [RIGHT_LEG_TRANS, 0], True)
            packed += shift_img(body_tile, s, s2, s, s2, np.logical_or(body_region_mask == BODY_COLOR, body_region_mask == BUTT_COLOR), BODY_TRANS)
            packed += shift_img(head_tile, s, s2, 0, s, head_region_mask == LIP_COLOR, LIP_TRANS)
            # packed += shift_img(head, s, s2, s, s2, head_region_mask == HEAD_COLOR, [0.008526, 0.019377])
            packed[s:, :s] = head_tile
            if nails_tile is not None:
                packed[s2 - s4:s2, s:s + s4] = nails_tile
            if mouth_tile is not None:
                packed[s2 - s4:s2, s+s4:s + s4*2] = mouth_tile
            if eyes_tile is not None:
                packed[s2 - s4 - s8:s2 - s4, s + s4 * 1:s + s4 * 2] = eyes_tile[:s8]
                packed[s2 - s4 - s8:s2 - s4, s + s4 * 2:s + s4 * 3] = eyes_tile[s8:]
            if gp_tile is not None:
                packed[s2 - s4:s2, s+s4*2:s + s4*3] = gp_tile


            # packed[:s, :s] = legs_tile
            # packed[s:, s:] = body_tile
            # packed[:s, s:] = arms_tile
            packed = Image.fromarray(packed)
            packed.save(self.get_concat_image_path(map_type))
            # plt.imshow(packed)
            # plt.show()


    def merge_geografts(self):
        BODY_M = self.get_body_mesh()
        BODY_RIG = self.get_body_rig()
        select_object(BODY_M)
        # merge meshes
        if 'GoldenPalace_G9 Mesh' in bpy.data.objects:
            GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
            GOLD_PAL_M.select_set(True)
        if 'GoldenPalace_G9 Mesh' in bpy.data.objects:
            BREASTACULAR_M = bpy.data.objects['BreastacularG9 Mesh']
            BREASTACULAR_M.select_set(True)
        bpy.ops.daz.merge_geografts()

        # merge bones
        if 'GoldenPalace_G9' in bpy.data.objects:
            self.merge_two_rigs(BODY_RIG, bpy.data.objects['GoldenPalace_G9'])
        if 'BreastacularG9' in bpy.data.objects:
            self.merge_two_rigs(BODY_RIG,  bpy.data.objects['BreastacularG9'])

    def merge_two_rigs(self, original, addon):
        select_object(original)
        addon.select_set(True)


        deform_bones = {bone.name for bone in addon.data.bones if bone.use_deform}
        parents = {bone.name: bone.parent.name for bone in addon.data.bones if bone.parent is not None}

        bpy.ops.object.mode_set(mode='EDIT')
        duplicates = [bone for bone in addon.data.edit_bones if bone.name in original.data.bones]
        for dup in duplicates:
            addon.data.edit_bones.remove(dup)

        addon.select_set(True)
        original.select_set(True)
        bpy.context.view_layer.objects.active = original
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.join()
        for bone in original.data.bones:
            if bone.name in deform_bones:
                bone.use_deform = True
        bpy.ops.object.mode_set(mode='EDIT')
        for bone in original.data.edit_bones:
            parent = parents.get(bone.name)
            if parent is not None:
                bone.parent = original.data.edit_bones[parent]
        bpy.ops.object.mode_set(mode='OBJECT')

    def merge_eyes(self):
        if 'Genesis 9 Eyes Mesh' in bpy.data.objects:
            EYES_M = bpy.data.objects['Genesis 9 Eyes Mesh']
            BODY_M = self.get_body_mesh()

            BODY_RIG = self.get_body_rig()
            select_object(BODY_M)
            EYES_M.select_set(True)
            old_uv_maps = [o.name for o in EYES_M.data.uv_layers]
            # merge meshes
            bpy.ops.object.join()

            # merge UV maps
            eyes_layer = BODY_M.data.uv_layers[NEW_EYES_UV_MAP]
            eyes_layer_np = np.array([v.uv for v in eyes_layer.data])
            is_eye = np.all(eyes_layer_np > 0, axis=1)
            base_layer_np = self.get_base_uv_layer_np()
            base_layer_np[is_eye] = eyes_layer_np[is_eye] + [5, 0]
            self.update_base_uv_layer(base_layer_np)
            for o in old_uv_maps:
                BODY_M.data.uv_layers.remove(BODY_M.data.uv_layers[o])

            # merge bones
            if 'Genesis 9 Eyes' in bpy.data.objects:
                EYES_RIG = bpy.data.objects['Genesis 9 Eyes']
                self.merge_two_rigs(BODY_RIG, EYES_RIG)

    def merge_mouth(self):
        if 'Genesis 9 Mouth Mesh' in bpy.data.objects:
            MOUTH_M = bpy.data.objects['Genesis 9 Mouth Mesh']
            BODY_M = self.get_body_mesh()

            BODY_RIG = self.get_body_rig()
            select_object(BODY_M)
            uv_map_name = MOUTH_M.data.uv_layers.active.name
            # merge meshes
            MOUTH_M.select_set(True)
            bpy.ops.object.join()

            # merge UV maps
            mouth_layer = BODY_M.data.uv_layers[uv_map_name]
            mouth_layer_np = np.array([v.uv for v in mouth_layer.data])
            is_mouth = np.all(mouth_layer_np > 0, axis=1)
            base_layer_np = self.get_base_uv_layer_np()
            base_layer_np[is_mouth] = mouth_layer_np[is_mouth] + [6, 0]
            self.update_base_uv_layer(base_layer_np)
            BODY_M.data.uv_layers.remove(mouth_layer)

            if 'Genesis 9 Mouth' in bpy.data.objects:
                # merge bones
                MOUTH_RIG = bpy.data.objects['Genesis 9 Mouth']
                self.merge_two_rigs(BODY_RIG, MOUTH_RIG)

    def pack_uvs(self):

        # ========= Concat UVs =========
        BODY_M = self.get_body_mesh()
        # pack UVs
        select_object(BODY_M)
        base_layer_np = self.get_base_uv_layer_np()
        is_arms_legs_head_body = base_layer_np[:, 0] < 4
        is_nails = np.logical_and(4 < base_layer_np[:, 0], base_layer_np[:, 0] < 5)
        is_eyes = np.logical_and(5 < base_layer_np[:, 0], base_layer_np[:, 0] < 6)
        is_eyes_sclera = np.logical_and(is_eyes, base_layer_np[:, 1] > 0.5)
        is_eyes_iris = np.logical_and(is_eyes, base_layer_np[:, 1] < 0.5)
        is_mouth = np.logical_and(6 < base_layer_np[:, 0], base_layer_np[:, 0] < 7)
        base_layer_np[is_arms_legs_head_body] *= 0.5
        out_of_bounds = np.logical_and(1 < base_layer_np[:, 0], is_arms_legs_head_body)
        base_layer_np[out_of_bounds] += [-1, 0.5]
        gp_layer = BODY_M.data.uv_layers[NEW_GP_UV_MAP]
        gp_layer_np = np.array([v.uv for v in gp_layer.data])
        is_gp = np.all(gp_layer_np > 0, axis=1)
        nails_np = base_layer_np[is_nails]
        sclera_np = base_layer_np[is_eyes_sclera]
        iris_np = base_layer_np[is_eyes_iris]
        mouth_np = base_layer_np[is_mouth]
        gp_np = gp_layer_np[is_gp]

        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        def get_pixel_class():
            pixel_coords = np.copy(base_layer_np)
            pixel_coords[:, 1] = 1 - pixel_coords[:, 1]
            pixel_coords = (pixel_coords * uv_mask_size).clip(0, uv_mask_size - 1)
            pixel_coords = np.int32(pixel_coords)
            pix_cls = uv_mask[pixel_coords[:, 1], pixel_coords[:, 0]]
            return pix_cls

        pixel_class = get_pixel_class()

        s2 = 1
        s = 0.5
        s4 = 1/8
        s8 = 1/16
        base_layer_np[pixel_class == BOT_ARM_COLOR] += BOT_ARM_TRANS
        base_layer_np[pixel_class == TOP_ARM_COLOR] += TOP_ARM_TRANS
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1] = 1.5 - base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1]
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 0] += RIGHT_LEG_TRANS
        base_layer_np[np.logical_or(pixel_class == BODY_COLOR, pixel_class == BUTT_COLOR)] += BODY_TRANS
        base_layer_np[is_nails] = np.mod(nails_np,1) / 8 + [s, 0]
        base_layer_np[is_eyes_sclera] = np.mod(sclera_np, 1) / 8 + [s + s4 * 1, s4 - s8]
        base_layer_np[is_eyes_iris] = np.mod(iris_np, 1) / 8 + [s + s4 * 2, s4]
        base_layer_np[is_gp] = np.mod(gp_np, 1) / 8 + [s + s4 * 2, 0]
        base_layer_np[is_mouth] = np.mod(mouth_np, 1) / 8 + [s + s4, 0]
        self.update_base_uv_layer(base_layer_np)

    def separate_lip_uvs(self):
        BODY_M = self.get_body_mesh()
        # pack UVs
        select_object(BODY_M)
        bpy.ops.object.mode_set(mode='EDIT')

        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        # for v in bm.verts:
        #    v.select_set(False)

        for face in bm.faces:
            full_loop = True
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                uv[1] = 1 - uv[1]
                pixel_coord = (uv * uv_mask_size).clip(0, uv_mask_size - 1)
                pixel_coord = np.int32(pixel_coord)
                pixel_cls = uv_mask[pixel_coord[1], pixel_coord[0]]
                matched = pixel_cls == LIP_COLOR
                # loop_uv.select = matched
                full_loop = full_loop and matched
                # if matched:
                #    loop.vert.select_set(True)
            face.select_set(full_loop)

        # bm.select_mode = {'VERT', 'EDGE', 'FACE'}
        bm.select_flush_mode()
        # bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        bpy.ops.uv.select_all(action='SELECT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.uv.select_split()

        bpy.ops.object.mode_set(mode='OBJECT')
        #  def separate_lips(self):
        base_layer_np = self.get_base_uv_layer_np()
        # pixel_class = get_pixel_class()
        selection = self.get_base_uv_layer_selection_np()
        base_layer_np[selection] = base_layer_np[selection] + LIP_TRANS
        self.update_base_uv_layer(base_layer_np)
        # += [0.043945, 0.006836] # top arm
        # += [-0.072266 , 0.085937] # obttom arm
        # += [0.008526, 0.019377] # torso
        # *= 0.25# nails
        # -= 0.5 # nails

    def subdivide_breast_bones(self):

        BODY_RIG = self.get_body_rig()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_RIG.select_set(True)
        bpy.context.view_layer.objects.active = BODY_RIG
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.armature.select_all(action='DESELECT')
        cuts = 2
        for bone_name in ['r_pectoral', 'l_pectoral']:
            bone = BODY_RIG.data.edit_bones[bone_name]
            select_bone(bone)
        bpy.ops.armature.subdivide(number_cuts=cuts)
        l_pec_groups = []
        r_pec_groups = []
        pectorals = [('r_pectoral', r_pec_groups), ('l_pectoral', l_pec_groups)]
        for bone_name, vertex_groups in pectorals:
            for i in range(0, cuts):
                subbone_name = bone_name + "." + str(i + 1).zfill(3)
                subbone = BODY_RIG.data.edit_bones[subbone_name]
                subbone_name = subbone.name = bone_name + str(i + 1)
                group = bpy.context.object.vertex_groups.new(name=subbone_name)
                vertex_groups.append(group.index)
        BODY_M = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='EDIT')

        def contains_group(vertex, group_index):
            for g in vertex.groups:
                if g.group == group_index:
                    return g.weight
            return 0

        l_pec_idx = BODY_M.vertex_groups['l_pectoral'].index
        r_pec_idx = BODY_M.vertex_groups['r_pectoral'].index
        l_pec_weights = np.array([contains_group(v, l_pec_idx) for v in BODY_M.data.vertices])
        r_pec_weights = np.array([contains_group(v, r_pec_idx) for v in BODY_M.data.vertices])
        for pec_weights, pec_groups in [(l_pec_weights, l_pec_groups), (r_pec_weights, r_pec_groups)]:
            max_weight = np.max(pec_weights)
            for i, subpec_group in enumerate(l_pec_groups):
                subpec_weights = pec_weights - (i + 1) / (cuts + 1)

    def save_textures(self):
        BODY_M = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        tex_dir = self.textures_dir()
        if os.path.exists(tex_dir):
            shutil.rmtree(tex_dir)
        bpy.ops.daz.save_local_textures()

    def select_gp(self):
        mesh = bpy.data.objects['GoldenPalace_G9 Mesh']
        select_object(mesh)
        return mesh

    def select_body(self):
        mesh = self.get_body_mesh()
        select_object(mesh)
        return mesh

    def select_gp_or_body(self):
        if 'GoldenPalace_G9 Mesh' in bpy.data.objects:
            mesh = bpy.data.objects['GoldenPalace_G9 Mesh']
        else:
            mesh = self.get_body_mesh()
        select_object(mesh)
        return mesh

    def unify_golden_palace_uvs(self):
        mesh = self.select_gp_or_body()
        if NEW_GP_UV_MAP not in mesh.data.uv_layers:
            gp_labia_majora = mesh.data.uv_layers['Golden Palace 2']
            gp_labia_minora = mesh.data.uv_layers['Golden Palace']
            gp_labia_majora.active = True
            new_uv_layer = mesh.data.uv_layers.new(name=NEW_GP_UV_MAP)
            new_uv_layer_np = np.array([v.uv for v in new_uv_layer.data])
            gp_labia_majora_np = np.array([v.uv for v in gp_labia_majora.data])
            gp_labia_minora_np = np.array([v.uv for v in gp_labia_minora.data])
            is_majora = np.all(gp_labia_majora_np > 0, axis=1)
            is_minora = np.all(gp_labia_minora_np > 0, axis=1)
            gp_labia_majora_np = np.mod(gp_labia_majora_np, 1)
            gp_labia_minora_np = np.mod(gp_labia_minora_np, 1)
            is_labia_majora = np.logical_and(0.285 < gp_labia_majora_np[:, 0], gp_labia_majora_np[:, 0] < 0.72)
            vagina_symmetry_line = 0.26598
            vagina_extent = 0.47191
            vagina_half_width = vagina_extent - vagina_symmetry_line
            p1A = np.array([0.31444 - vagina_symmetry_line, 0.912306])
            p2A = np.array([0.4164 - vagina_symmetry_line, 0.545495])
            vag_distance = np.absolute(gp_labia_minora_np[:, 0] - vagina_symmetry_line)
            slopeA = (p2A[1] - p1A[1]) / (p2A[0] - p1A[0])
            # p1[2] = p1[0] * slope + offset
            # p1[2] - p1[0] * slope = offset
            offsetA = p1A[1] - p1A[0] * slopeA
            offsetA += 0.05  # just for a good measure to avoid errors due to floating point precision
            p1B = np.array([0.43189 - vagina_symmetry_line, 0.317139])
            p2B = np.array([0.40303 - vagina_symmetry_line, 0.217741])
            slopeB = (p2B[1] - p1B[1]) / (p2B[0] - p1B[0])
            offsetB = p1B[1] - p1B[0] * slopeB
            offsetB -= 0.1  # just for a good measure to avoid errors due to floating point precision
            is_labia_minora = np.logical_and(vag_distance * slopeB + offsetB < gp_labia_minora_np[:, 1],
                                             gp_labia_minora_np[:, 1] < vag_distance * slopeA + offsetA)
            is_labia_majora = np.logical_and(is_labia_majora, np.logical_not(is_labia_minora))
            is_anus = np.logical_and(is_minora, is_majora)
            is_anus = np.logical_and(is_anus, np.logical_not(np.logical_or(is_labia_majora, is_labia_minora)))
            p1 = (0.514551, 0.546842)  # point on circle boundary
            p2 = (0.499947, 0.550254)  # center
            p3 = (0.500005, 0.588271)  # oval top point
            vag_oval_longer_radius = np.linalg.norm(np.subtract(p3, p2))
            vag_radius = np.linalg.norm(np.subtract(p1, p2))
            vag_distance = gp_labia_majora_np - p2
            vag_distance[:, 1] *= vag_radius / vag_oval_longer_radius
            is_vagina = np.linalg.norm(vag_distance, axis=1) < vag_radius
            is_vagina = np.logical_and(is_vagina, np.logical_not(is_labia_minora))
            is_insides = np.logical_or(is_vagina, is_anus)
            new_uv_layer_np[:, :] = 0
            vagina_margin = 0.08
            new_uv_layer_np[is_labia_minora] = gp_labia_minora_np[is_labia_minora] - [vagina_margin, 0]
            new_uv_layer_np[is_labia_majora] = gp_labia_majora_np[is_labia_majora] + [1 - 0.72, 0]
            new_uv_layer_np[is_insides] = gp_labia_minora_np[is_insides] * (1 / 8) + [vagina_half_width * 2 - vagina_margin, 0]
            for v, new_uv in zip(new_uv_layer.data, new_uv_layer_np):
                v.uv = new_uv

    def simplify_golden_palace_material(self):
        mesh = self.select_gp_or_body()
        filepaths = {}
        for channel in ['Base Color', 'Roughness', 'Normal']:
            name = 'GP_Baked_' + channel
            if name in bpy.data.images:
                filepaths[channel] = bpy.data.images[name]
            else:
                p = os.path.join(self.workdir, self.name + "_" + channel + '_gp_baked.png')
                filepaths[channel] = bpy.data.images.load(p)
        for mat in mesh.data.materials:
            if mat.name.startswith("GP_"):
                mat.node_tree.nodes.clear()
                NodesUtils.gen_simple_material(mat.node_tree, filepaths, uvs=NEW_GP_UV_MAP)


    def setup_golden_palace_for_baking(self):
        mesh = self.select_gp_or_body()

        bpy.context.scene.render.engine = 'CYCLES'
        bpy.context.scene.cycles.device = 'GPU'
        bpy.context.scene.view_settings.view_transform = 'Standard'

        mesh.data.uv_layers[NEW_GP_UV_MAP].active = True
        baked_gp_imgs = {}
        for idx, channel in enumerate(['Base Color', 'Roughness', 'Normal']):
            name = 'GP_Baked_'+channel
            if 'GP_Baked' in bpy.data.images:
                baked_gp_img = bpy.data.images[name]
            else:
                gp_baked_path = os.path.join(self.workdir, self.name + "_" + channel + '_gp_baked.png')
                if os.path.exists(gp_baked_path):
                    baked_gp_img = bpy.data.images.load(gp_baked_path)
                    baked_gp_img.name = name
                else:
                    baked_gp_img = bpy.data.images.new(name, 1024 * 4, 1024 * 4)
                baked_gp_img.colorspace_settings.name = 'sRGB' if channel == 'Base Color' else 'Non-Color'
            baked_gp_imgs[channel] = baked_gp_img
        for mat in mesh.data.materials:
            if mat.name.startswith('GP_'):
                n = mat.node_tree.nodes
                l = mat.node_tree.links
                uv_map = n.new('ShaderNodeUVMap')
                uv_map.location = (-300, 200)
                uv_map.uv_map = NEW_GP_UV_MAP
                uv_map.name = 'GP_Baked_UVs'
                bsdf_node = n['simple_material_bsdf']
                diffuse_node = n.new('ShaderNodeBsdfDiffuse')
                diffuse_node.location = bsdf_node.location
                diffuse_node.location.x -= 300
                diffuse_node.location.y += 200
                diffuse_node.name = 'simple_material_diffuse'
                for idx, channel in enumerate(['Base Color', 'Roughness', 'Normal']):
                    bsdf_links = bsdf_node.inputs[channel].links
                    before_bsdf = bsdf_links[0].from_socket
                    diffuse_socket_name = 'Color' if channel == 'Base Color' else channel
                    l.new(diffuse_node.inputs[diffuse_socket_name], before_bsdf)

                    name = 'GP_Baked_' + channel
                    target_texture = n.new('ShaderNodeTexImage')
                    target_texture.image = baked_gp_imgs[channel]
                    target_texture.name = name+' Texture'
                    target_texture.location = (0, 200 + 300 * idx)
                    l.new(target_texture.inputs['Vector'], uv_map.outputs['UV'])
                    n.active = target_texture
                l.new(bsdf_node.outputs['BSDF'].links[0].to_socket, diffuse_node.outputs['BSDF'])


    def select_golden_palace_for_bsdf_mode_baking(self, principled_bsdf):
        mesh = self.select_gp_or_body()
        for mat in mesh.data.materials:
            if mat.name.startswith('GP_'):
                n = mat.node_tree.nodes
                l = mat.node_tree.links
                bsdf_node = n['simple_material_bsdf']
                diffuse_node = n['simple_material_diffuse']
                good_node, bad_node = (bsdf_node, diffuse_node) if principled_bsdf else (diffuse_node, bsdf_node)
                bad_links = bad_node.outputs['BSDF'].links
                if len(bad_links) > 0:
                    l.new(bad_links[0].to_socket, good_node.outputs['BSDF'])


    def select_gp_color_for_baking(self):
        bpy.context.scene.cycles.bake_type = 'DIFFUSE'
        bpy.context.scene.render.bake.use_pass_direct = False
        bpy.context.scene.render.bake.use_pass_indirect = False
        self.select_golden_palace_for_baking('Base Color')
        self.select_golden_palace_for_bsdf_mode_baking(principled_bsdf=False)

    def select_gp_normals_for_baking(self):
        bpy.context.scene.cycles.bake_type = 'NORMAL'
        self.select_golden_palace_for_baking('Normal')
        self.select_golden_palace_for_bsdf_mode_baking(principled_bsdf=True)

    def select_gp_roughness_for_baking(self):
        bpy.context.scene.cycles.bake_type = 'ROUGHNESS'
        self.select_golden_palace_for_baking('Roughness')
        self.select_golden_palace_for_bsdf_mode_baking(principled_bsdf=True)

    def select_golden_palace_for_baking(self, channel):
        mesh = self.select_gp_or_body()
        for mat in mesh.data.materials:
            if mat.name.startswith('GP_'):
                n = mat.node_tree.nodes
                l = mat.node_tree.links
                target_texture = n[ 'GP_Baked_' + channel+' Texture']
                target_texture.select = True
                n.active = target_texture
                # color_texture = n['simple_material_Base Color']
                # bsdf_node = n['simple_material_bsdf']
                # bsdf_out = bsdf_node.outputs['BSDF']
                # color_out = color_texture.outputs['Color']
                # is_bsdf_connected = len(bsdf_out.links)>0
                # if channel == 'Base Color':
                #     if is_bsdf_connected:
                #         after_bsdf = bsdf_out.links[0].to_socket
                #         l.remove(bsdf_out.links[0])
                #         l.new(after_bsdf, color_out)
                # else:
                #     if not is_bsdf_connected:
                #         for color_link in color_out.links:
                #             if color_link.to_node != bsdf_node:
                #                 after_bsdf = color_link.to_socket
                #                 l.remove(color_link)
                #                 l.new(after_bsdf, bsdf_out)
                #                 break


    def save_gp_textures(self):
        for channel in ['Base Color', 'Roughness', 'Normal']:
            gp_baked_path = os.path.join(self.workdir, self.name +"_"+ channel+'_gp_baked.png')
            name = 'GP_Baked_' + channel
            if name in bpy.data.images:
                bpy.data.images[name].save(filepath=gp_baked_path)


    def make_single_material(self):
        body_m = self.select_body()
        mat = NodesUtils.remove_all_mats(body_m, "UnifiedSkin")
        filepaths = {}
        for channel in ['Base Color', 'Roughness', 'Normal']:
            filepaths[channel] = self.get_concat_image_path(channel)
        NodesUtils.gen_simple_material(mat.node_tree, filepaths)
        old_uv_maps = [o.name for o in body_m.data.uv_layers]
        for uv_layer_name in old_uv_maps:
            if uv_layer_name != 'Base Multi UDIM':
                l = body_m.data.uv_layers[uv_layer_name]
                body_m.data.uv_layers.remove(l)

    def convert_daz_to_ue5_skeleton(self):
        body_rig = self.get_body_rig()
        body_mesh = self.get_body_mesh()
        def convert_rig(rig, mid = None):
            select_object(rig)
            bpy.ops.object.mode_set(mode='EDIT')
            pelvis = None
            if mid is None:
                pelvis = rig.data.edit_bones['pelvis']
                pelvis_head = np.array(pelvis.head)
                pelvis_tail = np.array(pelvis.tail)
                t = 0.80
                mid = pelvis_tail * t + pelvis_head * (1 - t)
            if 'pelvis' in rig.data.edit_bones:
                pelvis = rig.data.edit_bones['pelvis']
                pelvis_head = np.array(pelvis.head)
                pelvis.head = mid
                pelvis.tail = pelvis_head
            if 'hip' in rig.data.edit_bones:
                hip = rig.data.edit_bones['hip']
                hip_tail = np.array(hip.tail)
                hip.head = hip_tail
                hip.tail = mid
                if pelvis is not None:
                    pelvis_children = list(pelvis.children)
                    for c in pelvis_children:
                        c.parent = hip
            if 'spine1' in rig.data.edit_bones and pelvis is not None:
                rig.data.edit_bones['spine1'].parent = pelvis
            for daz_name, ue5_name in DAZ_G9_TO_UE5_BONES.items():
                if daz_name in rig.data.edit_bones:
                    bone = rig.data.edit_bones[daz_name]
                    bone.name = bone.name + "_tmp_suffix"
            for daz_name, ue5_name in DAZ_G9_TO_UE5_BONES.items():
                daz_name = daz_name + "_tmp_suffix"
                if daz_name in rig.data.edit_bones:
                    bone = rig.data.edit_bones[daz_name]
                    bone.name = ue5_name
            return mid

        mid_pelvis_loc = convert_rig(body_rig)
        body_rig.name = 'root'
        body_mesh.name = 'root Mesh'
        for other_rig in bpy.data.objects:
            if isinstance(other_rig.data, bpy.types.Armature):
                convert_rig(other_rig, mid=mid_pelvis_loc)

    def add_ue5_ik_bones(self):
        rig = self.get_body_rig()
        select_object(rig)
        bpy.ops.object.mode_set(mode='EDIT')
        for ik_bone_name, fk_bone_name in UE5_IK_BONES.items():
            ik_bone = rig.data.edit_bones.new(ik_bone_name)
            ik_bone.use_deform = False
            ik_bone.use_connect = False
            if fk_bone_name != '':
                head = np.array(rig.data.edit_bones[fk_bone_name].head)
                ik_bone.head = head
            ik_bone.tail = np.array(ik_bone.head)
            ik_bone.tail.y += 0.2
        for ik_bone_name in UE5_IK_BONES:
            parent_ik_bone_name = UE5_BONE_HIERARCHY[ik_bone_name]
            if parent_ik_bone_name != '':
                rig.data.edit_bones[ik_bone_name].parent = rig.data.edit_bones[parent_ik_bone_name]

    def scale_to_ue5_units(self):
        s = bpy.context.scene.unit_settings.scale_length
        bpy.context.scene.unit_settings.scale_length = 0.01
        rig = self.get_body_rig()
        select_object(rig)
        z = s/0.01
        rig.scale = (z,z,z)
        stack = [rig]
        visited = {rig}
        while len(stack)>0:
            bpy.ops.object.select_all(action='DESELECT')
            obj = stack.pop()
            obj.select_set(True)
            bpy.context.view_layer.objects.active = obj
            bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
            for child in obj.children:
                if child not in visited:
                    visited.add(child)
                    stack.append(child)

    def export_to_fbx(self):
        body = self.get_body_mesh()
        rig = self.get_body_rig()
        select_object(rig)
        body.select_set(True)

        if "Subsurf" in body.modifiers:
            body.modifiers.remove(body.modifiers["Subsurf"])

        p = os.path.join(self.workdir, self.name + '.fbx')
        bpy.ops.export_scene.fbx(filepath=p,
                                 check_existing=False,
                                 filter_glob='*.fbx',
                                 use_selection=True,
                                 use_visible=False,
                                 use_active_collection=False,
                                 collection='',
                                 global_scale=1.0,
                                 apply_unit_scale=True,
                                 apply_scale_options='FBX_SCALE_NONE',
                                 use_space_transform=True,
                                 bake_space_transform=False,
                                 object_types={'ARMATURE', 'CAMERA', 'EMPTY', 'LIGHT', 'MESH', 'OTHER'},
                                 use_mesh_modifiers=True,
                                 use_mesh_modifiers_render=True,
                                 mesh_smooth_type='FACE',
                                 colors_type='SRGB',
                                 prioritize_active_color=False,
                                 use_subsurf=False,
                                 use_mesh_edges=False,
                                 use_tspace=False,
                                 use_triangles=False,
                                 use_custom_props=False,
                                 add_leaf_bones=False,
                                 primary_bone_axis='Y',
                                 secondary_bone_axis='X',
                                 use_armature_deform_only=False,
                                 armature_nodetype='NULL',
                                 bake_anim=False,
                                 bake_anim_use_all_bones=True,
                                 bake_anim_use_nla_strips=True,
                                 bake_anim_use_all_actions=True,
                                 bake_anim_force_startend_keying=True,
                                 bake_anim_step=1.0,
                                 bake_anim_simplify_factor=1.0,
                                 path_mode='AUTO',
                                 embed_textures=False,
                                 batch_mode='OFF',
                                 use_batch_own_dir=True,
                                 use_metadata=True,
                                 axis_forward='-Z',
                                 axis_up='Y')



    # *= 0.25# nails
    # -= 0.5 # nails


def save_blend_file(duf_filepath):
    duf_filepath = os.path.abspath(duf_filepath)
    workdir = os.path.dirname(duf_filepath)
    name = os.path.basename(duf_filepath)[:-len(".duf")]
    blend_filepath = os.path.join(workdir, name + ".blend")
    bpy.ops.wm.save_as_mainfile(filepath=blend_filepath)



def run_outside_blender():
    import subprocess

    file_path = os.path.realpath(__file__)

    if len(sys.argv) < 2 or not sys.argv[1].endswith(".duf"):
        print("Specify path to .duf")
        exit()
    duf_path: str = os.path.abspath(sys.argv[1])
    if not os.path.exists(duf_path):
        print("File not exists:", duf_path)
        exit()
    subprocess.run(["blender", "-P", file_path, "--", duf_path])

    # Image("")


bl_info = {
    "name": "Daz Optimizer",
    "blender": (2, 80, 0),
    "category": "Object",
}


class EasyImportPanel(bpy.types.Panel):
    bl_label = "Panel"
    bl_idname = "dazoptim_easy_import_panel"
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOLS'
    filepath = None
    directory = None

    @classmethod
    def poll(cls, context):
        op = context.active_operator
        if op and op.bl_idname == "DAZ_OT_easy_import_daz":
            cls.directory = op.directory
            cls.filepath = op.filepath
            #context.scene['duf_filepath'] = op.filepath
        return False

    # needs a draw method
    def draw(self, context):
        pass

UNLOCK = True

class DazDelCube_operator(bpy.types.Operator):
    """ Delete default cube """
    bl_idname = "dazoptim.delcube"
    bl_label = "Delete default cube"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK or 'daz_optim_stage' not in context.scene # context.mode == "OBJECT"

    def execute(self, context):
        for x in list(bpy.data.objects):
            bpy.data.objects.remove(x)
        for x in list(bpy.data.collections):
            bpy.data.collections.remove(x)
        context.scene['daz_optim_stage'] = 0
        return {'FINISHED'}


class DazLoad_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.load"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}
    idx = 1
    @classmethod
    def poll(cls, context):
        return UNLOCK or 'daz_optim_stage' not in context.scene or int(context.scene['daz_optim_stage']) < DazLoad_operator.idx

    def execute(self, context):
        bpy.ops.daz.easy_import_daz('INVOKE_DEFAULT',
                                    # filepath=self.duf_path,
                                    # files=[filepath],
                                    # directory="",
                                    filter_glob="*.duf;*.dsf;*.png;*.jpeg;*.jpg;*.bmp",
                                    fitMeshes='DBZFILE',
                                    materialMethod='EXTENDED_PRINCIPLED',
                                    useMergeMaterials=True,
                                    useEliminateEmpties=True,
                                    useMergeRigs=False,
                                    useApplyTransforms=False,
                                    useMergeToes=False,
                                    useFavoMorphs=False,
                                    useUnits=False,
                                    useExpressions=False,
                                    useVisemes=False,
                                    useHead=False,
                                    useFacs=False,
                                    useFacsdetails=False,
                                    useFacsexpr=False,
                                    useAnime=False,
                                    usePowerpose=False,
                                    useBody=False,
                                    useBulges=False,
                                    useJcms=True,
                                    ignoreFingers=True,
                                    useMasculine=False,
                                    useFeminine=False,
                                    useFlexions=True,
                                    useBakedCorrectives=True,
                                    useDazFavorites=True,
                                    useAdjusters=False,
                                    onMorphSuffix='SMART',
                                    useTransferFace=True,
                                    useTransferHair=False,
                                    useTransferGeografts=True,
                                    useTransferClothes=False,
                                    useTransferHD=True,
                                    useMergeGeografts=False,
                                    useMakePosable=True,
                                    useFinalOptimization=False,
                                    ignoreUrl=False,
                                    ignoreFinger=False,
                                    morphSuffix="",
                                    ignoreHdMorphs=False,
                                    useMhxOnly=False,
                                    duplicateDistance=1,
                                    useMergeNonConforming='CONTROLS',
                                    useConvertWidgets=True,
                                    useHiddenRigs=False,
                                    useMergeUvs=True,
                                    allowOverlap=False,
                                    keepOriginal=False,
                                    useFixTiles=True,
                                    useSubDDisplacement=True,
                                    useGeoNodes=False,
                                    morphStrength=1,
                                    skinColor=(0.6, 0.4, 0.25, 1),
                                    clothesColor=(0.09, 0.01, 0.015, 1),
                                    useApplyRestPoses=True,
                                    favoPath="")
        context.scene['daz_optim_stage'] = DazLoad_operator.idx
        return {'FINISHED'}


class DazSaveBlend_operator(bpy.types.Operator):
    bl_idname = "dazoptim.save_blend"
    bl_label = "Save blend file"
    bl_options = {"REGISTER", "UNDO"}
    idx = 2
    @classmethod
    def poll(cls, context):
        return UNLOCK or int(context.scene['daz_optim_stage']) == DazLoad_operator.idx

    def execute(self, context):
        if bpy.types.dazoptim_easy_import_panel.filepath is None and 'duf_filepath' not in bpy.context.scene:
            self.report({"WARNING"}, "Load a DAZ character first!")
            return {'CANCELLED'}
        if bpy.types.dazoptim_easy_import_panel.filepath is not None and 'duf_filepath' not in bpy.context.scene:
            bpy.context.scene['duf_filepath'] = bpy.types.dazoptim_easy_import_panel.filepath
        save_blend_file(bpy.context.scene['duf_filepath'])
        context.scene['daz_optim_stage'] = DazSaveBlend_operator.idx
        return {'FINISHED'}


class DazSaveTextures_operator(bpy.types.Operator):
    bl_idname = "dazoptim.save_textures"
    bl_label = "Save Daz textures"
    bl_options = {"REGISTER", "UNDO"}
    idx = 3
    @classmethod
    def poll(cls, context):
        return UNLOCK or int(context.scene['daz_optim_stage']) == DazSaveBlend_operator.idx #context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().save_textures()
        return {'FINISHED'}


class DazMergeAllRigs_operator(bpy.types.Operator):
    bl_idname = "dazoptim.merge_all_rigs"
    bl_label = "Merge all rigs (except hair)"
    bl_options = {"REGISTER", "UNDO"}
    idx = 3
    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().merge_all_rigs()
        return {'FINISHED'}

class DazSelectGoldenPalaceColor_operator(bpy.types.Operator):
    bl_idname = "dazoptim.select_gp_color"
    bl_label = "Select golden palace base color for baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().select_gp_color_for_baking()
        return {'FINISHED'}


class DazGoldenPalaceBsdf_operator(bpy.types.Operator):
    bl_idname = "dazoptim.gp_baking_principled"
    bl_label = " golden palace use principle bsdf for baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().select_golden_palace_for_bsdf_mode_baking(True)
        return {'FINISHED'}

class DazGoldenPalaceDiffuse_operator(bpy.types.Operator):
    bl_idname = "dazoptim.gp_baking_diffuse"
    bl_label = " golden palace use diffuse node for baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().select_golden_palace_for_bsdf_mode_baking(False)
        return {'FINISHED'}

class DazSelectGoldenPalaceNormals_operator(bpy.types.Operator):
    bl_idname = "dazoptim.select_gp_normal"
    bl_label = "Select golden palace normal maps for baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().select_gp_normals_for_baking()
        return {'FINISHED'}

class DazSelectGoldenPalaceRoughness_operator(bpy.types.Operator):
    bl_idname = "dazoptim.select_gp_roughness"
    bl_label = "Select golden palace roughness maps for baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().select_gp_roughness_for_baking()
        return {'FINISHED'}

class DazSaveGoldenPalaceBaked_operator(bpy.types.Operator):
    bl_idname = "dazoptim.save_gp_baked"
    bl_label = "Save baked golden palace textures"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().save_gp_textures()
        return {'FINISHED'}

class DazSimplifyGoldenPalaceMaterials_operator(bpy.types.Operator):
    bl_idname = "dazoptim.simplify_gp_mats"
    bl_label = "Simplify golden palace materials after baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().simplify_golden_palace_material()
        return {'FINISHED'}


class DazSimplifyMaterials_operator(bpy.types.Operator):
    """ Simplify materials """
    bl_idname = "dazoptim.simpl_mats"
    bl_label = "Simplify materials"
    bl_options = {"REGISTER", "UNDO"}
    idx = 3

    @classmethod
    def poll(cls, context):
        return UNLOCK or DazSaveBlend_operator.idx <= int(context.scene['daz_optim_stage']) < DazSimplifyMaterials_operator.idx

    def execute(self, context):
        DazOptimizer().simplify_materials()

        return {'FINISHED'}



class DazOptimizeEyes_operator(bpy.types.Operator):
    """ Optimize eyes """
    bl_idname = "dazoptim.optim_eyes"
    bl_label = "Optimize eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().optimize_eyes()

        return {'FINISHED'}

class DazSimplifyEyesMaterial_operator(bpy.types.Operator):
    """ Simplify eyes material """
    bl_idname = "dazoptim.simpl_eyes_mat"
    bl_label = "Simplify eyes material"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().simplify_eyes_material()

        return {'FINISHED'}

class DazSeparateIrisUVs_operator(bpy.types.Operator):
    """ Separate iris UVs """
    bl_idname = "dazoptim.sep_iris_uvs"
    bl_label = "Separate iris UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().separate_iris_uvs()

        return {'FINISHED'}

class DazMergeGrografts_operator(bpy.types.Operator):
    """ Merge geografts """
    bl_idname = "dazoptim.merge_geografts"
    bl_label = "Merge geografts"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_geografts()

        return {'FINISHED'}


class DazMergeEyes_operator(bpy.types.Operator):
    """ Merge Eyes """
    bl_idname = "dazoptim.merge_eyes"
    bl_label = "Merge eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_eyes()

        return {'FINISHED'}


class DazMergeMouth_operator(bpy.types.Operator):
    """ Merge Mouth """
    bl_idname = "dazoptim.merge_mouth"
    bl_label = "Merge mouth"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_mouth()

        return {'FINISHED'}


class DazConcatTextures_operator(bpy.types.Operator):
    """ Concatenate textures into one """
    bl_idname = "dazoptim.concat"
    bl_label = "Concatenate textures"
    bl_options = {"REGISTER", "UNDO"}
    idx = 4

    @classmethod
    def poll(cls, context):
        return UNLOCK or DazSimplifyMaterials_operator.idx <= int(context.scene['daz_optim_stage']) < DazConcatTextures_operator.idx

    def execute(self, context):
        DazOptimizer().concat_textures()

        return {'FINISHED'}

class DazOptimizeUVs_operator(bpy.types.Operator):
    """ Optimize UVs """
    bl_idname = "dazoptim.optim_uvs"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazMakeSingleMaterial_operator(bpy.types.Operator):
    """ Make a single unified skin material """
    bl_idname = "dazoptim.single_material"
    bl_label = "Make a single unified skin material"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK # context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().make_single_material()

        return {'FINISHED'}


class DazSeparateLipUVs_operator(bpy.types.Operator):
    """ Separate Lip UVs """
    bl_idname = "dazoptim.sep_lip_uvs"
    bl_label = "Separate Lip UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().separate_lip_uvs()

        return {'FINISHED'}


class DazAddBreastBones_operator(bpy.types.Operator):
    """ Add breast bones """
    bl_idname = "dazoptim.breast_bones"
    bl_label = "Subdivide breast bones"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().subdivide_breast_bones()

        return {'FINISHED'}


class DazAddGluteBones_operator(bpy.types.Operator):
    """ Add glute bones """
    bl_idname = "dazoptim.glute_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):

        return {'FINISHED'}


class DazAddThighBones_operator(bpy.types.Operator):
    """ Add thigh bones """
    bl_idname = "dazoptim.thigh_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):

        return {'FINISHED'}


class DazFitClothes_operator(bpy.types.Operator):
    """ fit clothes """
    bl_idname = "dazoptim.fit_clothes"
    bl_label = "Fit clothes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):

        return {'FINISHED'}


class DazOptimizeHair_operator(bpy.types.Operator):
    """ Optimize hair """
    bl_idname = "dazoptim.optim_hair"
    bl_label = "Optimize Hair"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):

        return {'FINISHED'}


class DazConvertToUe5Skeleton_operator(bpy.types.Operator):
    """ Convert rig to UE5-compatible skeleton """
    bl_idname = "dazoptim.convert_ue5"
    bl_label = "Convert rig to UE5-compatible skeleton"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().convert_daz_to_ue5_skeleton()

        return {'FINISHED'}


class DazOptimizeGoldenPalaceUVs(bpy.types.Operator):
    """ Optimize Golden Palace UVs """
    bl_idname = "dazoptim.optim_gp_uvs"
    bl_label = "Optimize Golden Palace UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().unify_golden_palace_uvs()

        return {'FINISHED'}


class DazSetupGoldenPalaceForBaking(bpy.types.Operator):
    """ Setup Golden Palace for Baking """
    bl_idname = "dazoptim.setup_gp_bake"
    bl_label = "Setup Golden Palace fro baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().setup_golden_palace_for_baking()
        return {'FINISHED'}


class DazBakeGoldenPalace(bpy.types.Operator):
    """ Bake Golden Palace """
    bl_idname = "dazoptim.bake_gp"
    bl_label = "Bake Golden Palace"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        bpy.ops.object.bake(type='DIFFUSE')
        return {'FINISHED'}

class AddUe5IkBones(bpy.types.Operator):
    """ Add UE5 IK bones """
    bl_idname = "dazoptim.add_ue5_ik_bones"
    bl_label = "Add UE5 IK bones"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().add_ue5_ik_bones()

        return {'FINISHED'}

class DazScaleToUnreal(bpy.types.Operator):
    """ Scale to unreal """
    bl_idname = "dazoptim.scale_to_ue5"
    bl_label = "Scale to unreal"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().scale_to_ue5_units()

        return {'FINISHED'}

class DazExportFbx(bpy.types.Operator):
    """ export fbx """
    bl_idname = "dazoptim.export_fbx"
    bl_label = "Export fbx"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return UNLOCK

    def execute(self, context):
        DazOptimizer().export_to_fbx()

        return {'FINISHED'}


class DazOptimize_sidebar(bpy.types.Panel):
    """DazOptim actions"""
    bl_label = "DazOptim"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "DazOptim"

    def draw(self, context):
        col = self.layout.column(align=True)
        i = 0
        for op_class, op_text in operators:
            prop = col.operator(op_class.bl_idname, text=str(i) + ". " + op_text)
            op_class.idx = i
            i += 1


operators = [
    (DazDelCube_operator, "Delete default cube"),
    (DazLoad_operator, "Load Daz"),
    (DazSaveBlend_operator, "Save blend file"),
    (DazSaveTextures_operator, "Save textures"),
    (DazMergeAllRigs_operator, "Merge all rigs"),
    (DazSimplifyMaterials_operator, "Simplify materials"),
    (DazOptimizeEyes_operator, "Optimize eyes mesh"),
    (DazSimplifyEyesMaterial_operator, "Simplify eyes material"),
    (DazSeparateIrisUVs_operator, "Separate iris UVs"),
    (DazOptimizeGoldenPalaceUVs, "Optimize golden palace UVs"),
    (DazSetupGoldenPalaceForBaking, "golden palace prepare baking"),
    (DazSelectGoldenPalaceColor_operator, "select golden palace color for baking"),
    (DazSelectGoldenPalaceNormals_operator, "select golden palace normals for baking"),
    (DazSelectGoldenPalaceRoughness_operator, "select golden palace roughness for baking"),
    (DazGoldenPalaceBsdf_operator, "use principled bsdf"),
    (DazGoldenPalaceDiffuse_operator, "use diffuse bsdf"),
    (DazSaveGoldenPalaceBaked_operator, "Save baked golden palace textures"),
    (DazMergeGrografts_operator, "Merge Geografts"),
    (DazSimplifyGoldenPalaceMaterials_operator, "Simplify golden palace materials"),
    (DazMergeEyes_operator, "Merge eyes"),
    (DazMergeMouth_operator, "Merge mouth"),
    (DazConcatTextures_operator, "Merge textures"),
    (DazOptimizeUVs_operator, "Optimize UVs"),
    (DazSeparateLipUVs_operator, "Separate Lip UVs"),
    (DazMakeSingleMaterial_operator, "Unify skin materials into one"),
    (DazAddBreastBones_operator, "Subdivide breast bones"),
    (DazAddGluteBones_operator, "Add glute bones"),
    (DazAddThighBones_operator, "Add thigh bones"),
    (DazFitClothes_operator, "Fit Clothes"),
    (DazOptimizeHair_operator, "Optimize hair"),
    (DazConvertToUe5Skeleton_operator, "Convert to UE5 Skeleton"),
    (AddUe5IkBones, "Add UE5 IK bones"),
    (DazScaleToUnreal, "Scale to ue5 units"),
    (DazExportFbx, "Export to fbx"),
]

classes = [
              DazOptimize_sidebar,
              EasyImportPanel,
          ] + [op for (op, _) in operators]


def register():
    for c in classes:
        bpy.utils.register_class(c)


def unregister():
    for c in classes:
        bpy.utils.unregister_class(c)


if __name__ == '__main__':
    register()
